#pragma once

#include "eatkconfig.h"
#include "elitism.h"
#include "crossovermutation.h"

namespace eatk
{

class SingleBestElitism : public Elitism
{
public:
	SingleBestElitism(bool eliteWithoutMutation, const std::shared_ptr<GenomeMutation> &mutation);
	~SingleBestElitism();

	errut::bool_t check(const std::shared_ptr<SelectionPopulation> &selPop) override;
	errut::bool_t introduceElites(size_t generation, const std::shared_ptr<SelectionPopulation> &selPop,
								  const std::shared_ptr<Population> &population,
								  size_t targetPopulationSize) override;
private:
	bool m_eliteWithoutMutation;
	std::shared_ptr<GenomeMutation> m_mutation;
};

}
