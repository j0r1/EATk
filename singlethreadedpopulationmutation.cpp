#include "singlethreadedpopulationmutation.h"

using namespace std;
using namespace errut;

SingleThreadedPopulationMutation::SingleThreadedPopulationMutation(shared_ptr<GenomeMutation> mutation)
    : m_mutation(mutation)
{
}

SingleThreadedPopulationMutation::~SingleThreadedPopulationMutation()
{
}

bool_t SingleThreadedPopulationMutation::check(const vector<shared_ptr<Population>> &populations)
{
    for (auto &pop : populations)
    {
        for (auto &i : pop->m_individuals)
        {
            bool_t r = m_mutation->check(i->genomeRef());
            if (!r)
                return "Error in mutation check: " + r.getErrorString();
        }
    }

    return true;
}

bool_t SingleThreadedPopulationMutation::mutate(const vector<shared_ptr<Population>> &populations)
{
    for (auto &pop : populations)
    {
        size_t numToSkip = pop->getGenomesToSkipMutation();
        for (size_t idx = numToSkip ; idx < pop->m_individuals.size() ; idx++)
        {
            auto &i = pop->m_individuals[idx];
            bool isChanged = false;
            bool_t r = m_mutation->mutate(i->genomeRef(), isChanged);
            if (!r)
                return "Error in mutation: " + r.getErrorString();
            
            if (isChanged)
                i->fitnessRef().setCalculated(false);
        }
    }
    return true;
}