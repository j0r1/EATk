#include "vectordifferentialevolution.h"
#include "evolutionaryalgorithm.h"
#include "mersennerandomnumbergenerator.h"
#include "valuefitness.h"
#include "differentialevolutionevolver.h"
#include "singlethreadedpopulationfitnesscalculation.h"
#include <random>

using namespace std;
using namespace eatk;
using namespace errut;

template<class VT, class FT>
class VectorValueFitnessCalculation : public GenomeFitnessCalculation
{
public:
	VectorValueFitnessCalculation() : m_evaluations(0) { };

	errut::bool_t calculate(const Genome &genome, Fitness &fitness)
	{
		const VectorGenome<VT> &vg = static_cast<const VectorGenome<VT> &>(genome);
		ValueFitness<FT> &vf = static_cast<ValueFitness<FT> &>(fitness);

		m_evaluations++;

		vf.setValue(calculate(vg.getValues()));
		return true;
	}

	virtual FT calculate(const vector<VT> &x) = 0;
protected:
	size_t m_evaluations;
};

class BaseCalculation : public VectorValueFitnessCalculation<double,double>
{
public:
	size_t getNumberOfEvaluations() const { return m_evaluations; }
	void resetEvaluationCount() { m_evaluations = 0; }
};

class f1_Sphere : public BaseCalculation
{
public:
	double calculate(const vector<double> &x) override
	{
		assert(x.size() == 3);
		double sumSquared = 0;
		for (auto v : x)
			sumSquared += v*v;
		return sumSquared;
	}
};

class f2_Rosenbrock : public BaseCalculation
{
public:
	double calculate(const vector<double> &x) override
	{
		assert(x.size() == 2);
		double x1 = x[0];
		double x2 = x[1];

		return (100.0 * (x1*x1 - x2) * (x1*x1 - x2) + (1.0 - x1)*(1.0 - x1));
	}
};

class f8_Zimmermann : public BaseCalculation
{
public:
	double calculate(const vector<double> &x) override
	{
		assert(x.size() == 2);
		double x1 = x[0];
		double x2 = x[1];

		auto h1 = 9.0-x1-x2;
		auto h2 = (x1-3.0)*(x1-3.0) + (x2-2.0)*(x2-2.0) - 16.0;
		auto h3 = x1*x2 - 14.0;

		auto p = [](auto delta) { return 100.0 * (1.0 + delta); };
		auto sgn = [] (auto x) { return (x >= 0)?1.0:0.0; };

		return std::max(h1,
			std::max(
				std::max(p(h2)*sgn(h2),p(h3)*sgn(h3)), 
				std::max(p(-x1)*sgn(-x1),p(-x2)*sgn(-x2))
			)
		);
	}
};

template<class T>
class ValueToReachStop : public StopCriterion
{
public:
	ValueToReachStop(T value, size_t maxGenerations) : m_value(value), m_maxGen(maxGenerations),m_numGen(0)
	{
		m_dump = (getenv("DUMPPROGRESS"))?true:false;
	}

	bool_t analyze(const std::vector<std::shared_ptr<Individual>> &currentBest, size_t generationNumber, bool &shouldStop)
	{
		if (currentBest.size() != 1)
			return "Expecting current best size 1";
		
		if (m_dump)
			cerr << generationNumber << " " << currentBest[0]->toString() << endl;

		m_numGen = generationNumber;
		const ValueFitness<T> &vf = static_cast<const ValueFitness<T> &>(currentBest[0]->fitnessRef());
		if (vf.getValue() <= m_value)
		{
			shouldStop = true;
			m_best = currentBest[0]->createCopy();
		}
		else
		{
			if (generationNumber >= m_maxGen)
			{
				shouldStop = true;
				m_numGen = generationNumber;
			}
		}
		return true;			
	}

	size_t getNumberOfGenerations() const { return m_numGen; }
	bool converged() const { return (m_best.get())?true:false; }
	const Individual &bestSolution() const { return *m_best; }
private:
	const T m_value;
	const size_t m_maxGen;
	size_t m_numGen;
	shared_ptr<Individual> m_best;
	bool m_dump;
};

struct Test
{
	string name;
	size_t popSize;
	double F;
	double CR;
	vector<double> bottom;
	vector<double> top;
	double VTR; // Value to reach
	size_t maxGenerations;
	shared_ptr<BaseCalculation> calculator;
};

class MyEA : public EvolutionaryAlgorithm
{
public:
	MyEA()
	{
		m_dumpPop = (getenv("DUMPPOP"))?true:false;
	}
private:
	bool_t onFitnessCalculated(size_t generation, std::shared_ptr<Population> &population) override
	{
		if (m_dumpPop)
		{
			cerr << "Generation " << generation << ":" << endl;
			for (size_t i = 0 ; i < population->individuals().size() ; i++)
				cerr << "  [" << i << "] = " << population->individual(i)->toString() << endl;
			cerr << endl;
		}
		return true;
	}

	bool m_dumpPop;
};

int main(int argc, char const *argv[])
{
	random_device rndDev;
	unsigned int seed = rndDev();
	if (getenv("SEED"))
		seed = (unsigned int)stoul(getenv("SEED"));

	cout << "# Seed = " << seed << endl;

	shared_ptr<RandomNumberGenerator> rng = make_shared<MersenneRandomNumberGenerator>(seed);

	vector<Test> tests {
		{ "f1_Sphere",     10, 0.9, 0.1, { -5.12, -5.12, -5.12 }, { 5.12, 5.12, 5.12 }, 1e-6, 100000, make_shared<f1_Sphere>() },
		{ "f2_Rosenbrock", 20, 0.9, 0.9, { -2.048, -2.048 }, { 2.048, 2.048 }, 1e-6, 100000, make_shared<f2_Rosenbrock>() },
		{ "f8_Zimmermann", 20, 0.9, 0.9, { 0.0, 0.0 }, { 100.0, 100.0 }, 1e-6, 100000, make_shared<f8_Zimmermann>() },
	};

	auto comp = make_shared<ValueFitnessComparison<double>>();

	for (const auto &test : tests)
	{
		cout << "Running EA for: " << test.name << endl; 

		size_t numRuns = 20;
		for (size_t run = 0 ; run < numRuns ; run++)
		{
			auto mut = make_shared<VectorDifferentialEvolutionMutation<double>>(test.F);
			auto cross = make_shared<VectorDifferentialEvolutionCrossover<double>>(test.CR, rng);
			
			VectorDifferentialEvolutionIndividualCreation<double,double> creation(test.bottom, test.top, rng);
			ValueToReachStop<double> stop(test.VTR, test.maxGenerations);
		
			DifferentialEvolutionEvolver evolver(rng, mut, cross, comp);
			SingleThreadedPopulationFitnessCalculation popCalc(test.calculator);

			MyEA ea;
			bool_t r = ea.run(creation, evolver, popCalc, stop, test.popSize, test.popSize, test.popSize*2);
			if (!r)
			{
				cerr << r.getErrorString() << endl;
				return -1;
			}

			if (!stop.converged())
				cout << "  Failed to converge after " << stop.getNumberOfGenerations() << " generations" << endl;
			else
			{
				cout << "  Found after " << stop.getNumberOfGenerations() << " generations, " << test.calculator->getNumberOfEvaluations()
					 << " evaluations: " << stop.bestSolution().toString() << endl;
			}

			test.calculator->resetEvaluationCount();
		}
	}

	return 0;
}
