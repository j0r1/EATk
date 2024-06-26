#pragma once

#include "eatkconfig.h"
#include "nondominatedsetcreator.h"
#include "genomefitness.h"

namespace eatk
{

class BasicNonDominatedSetCreator : public NonDominatedSetCreator
{
public:
	BasicNonDominatedSetCreator(const std::shared_ptr<FitnessComparison> &fitCmp, size_t numObjectives);
	~BasicNonDominatedSetCreator();

	errut::bool_t calculateAllNDSets(const std::vector<std::shared_ptr<Individual>> &individuals,
	                                 size_t requestStopSize = std::numeric_limits<size_t>::max()) override;
	errut::bool_t calculateNonDomitatedSet(const std::vector<std::shared_ptr<Individual>> &individuals,
		std::vector<std::shared_ptr<Individual>> &ndSet,
		std::vector<std::shared_ptr<Individual>> &remaining) override;

	// Assumes that both are in fact ND sets
	errut::bool_t mergeNDSets(std::vector<std::shared_ptr<Individual>> &inOut, const std::vector<std::shared_ptr<Individual>> &added) override;

	size_t getNumberOfSets() const override { return m_sets.size(); }
	const std::vector<std::shared_ptr<Individual>> &getSet(size_t idx) const override { return m_sets[idx]; }
	std::vector<std::shared_ptr<Individual>> &getSet(size_t idx) override { return m_sets[idx]; }
private:
	size_t m_numObjectives;
	std::shared_ptr<FitnessComparison> m_cmp;
	
	std::vector<std::vector<std::shared_ptr<Individual>>> m_sets;
	std::vector<std::shared_ptr<Individual>> m_tmpND, m_tmpRem[2];
};

}