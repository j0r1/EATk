#pragma once

#include "mogal2config.h"
#include "genomefitness.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <limits>

namespace mogal2
{

// TODO: record parents?
// TODO: generation of creation? To allow age?
class Individual
{
public:
	Individual(std::shared_ptr<Genome> genome, std::shared_ptr<Fitness> fitness,
			   size_t introducedInGeneration = std::numeric_limits<size_t>::max())
		: m_genome(genome), m_fitness(fitness),
		  m_introducedInGeneration(introducedInGeneration), m_lastMutationGeneration(introducedInGeneration)
	{
	}

	void setLastMutationGeneration(size_t g)
	{
		assert(g >= m_introducedInGeneration);
		m_lastMutationGeneration = g;
	}

	std::string toString() const
	{
		return m_genome->toString() + ": " + m_fitness->toString() + "(" +
		       std::to_string(m_introducedInGeneration)+ "/" +
			   std::to_string(m_lastMutationGeneration) + ")";
	}

	std::shared_ptr<Individual> createCopy() const
	{
		auto ind = std::make_shared<Individual>(m_genome->createCopy(), m_fitness->createCopy(), m_introducedInGeneration);
		ind->m_lastMutationGeneration = m_lastMutationGeneration;
		return ind;
	}

	std::shared_ptr<Genome> &genome() { return m_genome; }
	Genome &genomeRef() 
	{ 
		assert(m_genome.get());
		return *m_genome;
	}
	Genome *genomePtr() { return m_genome.get(); }

	std::shared_ptr<Fitness> &fitness() { return m_fitness; }
	Fitness &fitnessRef()
	{
		assert(m_fitness.get());
		return *m_fitness;
	}

	Fitness *fitnessPtr() { return m_fitness.get(); }
private:
	std::shared_ptr<Genome> m_genome;
	std::shared_ptr<Fitness> m_fitness;
	size_t m_introducedInGeneration, m_lastMutationGeneration;
};

// Do we need this? Just a typedef perhaps?
class Population
{
public:
	Population() { }
	~Population() { }

	void clear() { m_individuals.clear(); }
	size_t size() const { return m_individuals.size(); }
	void append(const std::shared_ptr<Individual> &i)
	{
		assert(i.get());
		m_individuals.push_back(i);
	}
	void resize(size_t n) { m_individuals.resize(n); }

	const std::vector<std::shared_ptr<Individual>> &individuals() const { return m_individuals; }
	std::vector<std::shared_ptr<Individual>> &individuals() { return m_individuals; }
	std::shared_ptr<Individual> &individual(size_t n)
	{
		assert(n < m_individuals.size());
		return m_individuals[n];
	}

	void print() const
	{
		using namespace std;

		cout << "Population: " << endl;
	    for (auto &i : m_individuals)
    	    cout << i->toString() << endl;
    	cout << endl;
	}
private:
	std::vector<std::shared_ptr<Individual>> m_individuals;
};

class PopulationFitnessCalculation
{
public:
	PopulationFitnessCalculation() { }
	virtual ~PopulationFitnessCalculation() { }

	// This function can be called eg on first generation, to check that the types etc
	// are in order. This can then skip some checks in calculatePopulationFitness
	virtual errut::bool_t check(const std::vector<std::shared_ptr<Population>> &populations) { return "Not implemented in base class"; }

	// TODO: all populations should have exactly the same genomes! (ie same number of floats)
	virtual errut::bool_t calculatePopulationFitness(const std::vector<std::shared_ptr<Population>> &populations) { return "Not implemented in base class"; }
};

}
