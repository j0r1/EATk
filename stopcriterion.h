#pragma once

#include "population.h"

class StopCriterion
{
public:
    StopCriterion() { }
    virtual ~StopCriterion() { }

    virtual errut::bool_t analyze(const std::vector<std::shared_ptr<Individual>> &currentBest, size_t generationNumber, bool &shouldStop) { return "Not implemented in base class"; }
};

class FixedGenerationsStopCriterion : public StopCriterion
{
public:
    FixedGenerationsStopCriterion(size_t n) : m_maxGen(n) { }
    ~FixedGenerationsStopCriterion() { }

    errut::bool_t analyze(const std::vector<std::shared_ptr<Individual>> &currentBest, size_t generationNumber, bool &shouldStop) override
    {
        if (generationNumber >= m_maxGen)
            shouldStop = true;
        return true;
    }
private:
    size_t m_maxGen;
};
