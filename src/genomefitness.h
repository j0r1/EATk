#pragma once

#include "eatkconfig.h"

#ifdef EATKCONFIG_MPISUPPORT
#include <mpi.h>
#endif // EATKCONFIG_MPISUPPORT

#include <errut/booltype.h>
#include <memory>
#include <vector>

namespace eatk
{

template<class T>
class GenomeFitnessBase
{
protected:
	GenomeFitnessBase() { }
public:
	virtual ~GenomeFitnessBase() { }

	virtual std::string toString() const { return "?"; };

	virtual std::shared_ptr<T> createCopy(bool copyContents = true) const { return nullptr; }

#ifdef EATKCONFIG_MPISUPPORT
	virtual errut::bool_t MPI_BroadcastLayout(int root, MPI_Comm communicator) { return "Not implemented"; }
	virtual errut::bool_t MPI_Send(int dest, int tag, MPI_Comm communicator,
								   std::vector<MPI_Request> &requests) const { return "Not implemented"; }
	virtual errut::bool_t MPI_Recv(int src, int tag, MPI_Comm communicator,
								   std::vector<MPI_Request> &requests) { return "Not implemented"; }
#endif // EATKCONFIG_MPISUPPORT
};

class Genome : public GenomeFitnessBase<Genome>
{
public:
	Genome() { }
	~Genome() { }
};

class Fitness : public GenomeFitnessBase<Fitness>
{
public:
	Fitness() : m_calculated(false) { }
	~Fitness() { }

	bool isCalculated() const { return m_calculated; }
	void setCalculated(bool v = true) { m_calculated = v; }
protected:
	bool m_calculated;
};

class FitnessComparison
{
public:
	FitnessComparison() { }
	virtual ~FitnessComparison() { }

	virtual errut::bool_t check(const Fitness &f) const { return "Not implemented in base class"; }
	// Using just a bool for speed - will be used in sorting functions
	// TODO: perhaps some kind of FitnessComparison operator is better?
	virtual bool isFitterThan(const Fitness &first, const Fitness &second, size_t objectiveNumber) const { return false; } // Implement this
};

}
