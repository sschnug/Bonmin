// (C) Copyright International Business Machines Corporation and Carnegie Mellon University 2006
// All Rights Reserved.
// This code is published under the Common Public License.
//
// Authors :
// John J. Forrest, International Business Machines Corporation
// P. Bonami, Carnegie Mellon University,
//
// Date : 03/15/2006


#if defined(_MSC_VER)
// Turn off compiler warning about long names
#  pragma warning(disable:4786)
#endif
#include <string>
#include <cassert>
#include <cfloat>
#include "OsiSolverInterface.hpp"
#include "CoinWarmStartBasis.hpp"
#include "CbcModel.hpp"
#include "BonminCbcNode.hpp"
#include "IpoptInterface.hpp"
#include "IpoptWarmStart.hpp"

using namespace std;

//Default constructor
BonminCbcFullNodeInfo::BonminCbcFullNodeInfo()
    :
    CbcFullNodeInfo(),
    sequenceOfInfeasiblesSize_(0),
    sequenceOfUnsolvedSize_(0)
{}

BonminCbcFullNodeInfo::BonminCbcFullNodeInfo(CbcModel * model,
    int numberRowsAtContinuous) :
    CbcFullNodeInfo(model, numberRowsAtContinuous),
    sequenceOfInfeasiblesSize_(0),
    sequenceOfUnsolvedSize_(0)
{
}

// Copy constructor
BonminCbcFullNodeInfo::BonminCbcFullNodeInfo ( const BonminCbcFullNodeInfo &other):
    CbcFullNodeInfo(other),
    sequenceOfInfeasiblesSize_(other.sequenceOfInfeasiblesSize_),
    sequenceOfUnsolvedSize_(other.sequenceOfUnsolvedSize_)

{}


void
BonminCbcFullNodeInfo::allBranchesGone()
{
  IpoptWarmStart * ipws = dynamic_cast<IpoptWarmStart *>(basis_);
  if(ipws)
    ipws->flushPoint();
}

BonminCbcFullNodeInfo::~BonminCbcFullNodeInfo()
{}

CbcNodeInfo *
BonminCbcFullNodeInfo::clone() const
{
  return new BonminCbcFullNodeInfo(*this);
}
/****************************************************************************************************/

// Default constructor
BonminCbcPartialNodeInfo::BonminCbcPartialNodeInfo ()
    : CbcPartialNodeInfo(),
    sequenceOfInfeasiblesSize_(0),
    sequenceOfUnsolvedSize_(0)
{
}
// Constructor from current state
BonminCbcPartialNodeInfo::BonminCbcPartialNodeInfo (CbcModel * model,CbcNodeInfo *parent, CbcNode *owner,
    int numberChangedBounds,
    const int *variables,
    const double *boundChanges,
    const CoinWarmStartDiff *basisDiff)
    : CbcPartialNodeInfo(parent,owner,numberChangedBounds,variables,
        boundChanges,basisDiff),
    sequenceOfInfeasiblesSize_(0),
    sequenceOfUnsolvedSize_(0)
{
  IpoptInterface * ipopt = dynamic_cast<IpoptInterface *>(model->solver());
  assert (ipopt);
  Ipopt::ApplicationReturnStatus optimization_status
  = ipopt->getOptStatus();
  BonminCbcPartialNodeInfo * nlpParent = dynamic_cast<BonminCbcPartialNodeInfo *> (parent);
  int numberInfeasible = 0;
  int numberUnsolved = 0;
  if(nlpParent)//father is not root
  {
    numberInfeasible = nlpParent->getSequenceOfInfeasiblesSize();
    numberUnsolved =  nlpParent->getSequenceOfUnsolvedSize();
//       if(!nlpParent->numberBranchesLeft_){
// 	IpoptWarmStartDiff * ipws = dynamic_cast<IpoptWarmStartDiff *>(nlpParent->basisDiff_);
// 	ipws->flushPoint();
//       }
  }
  else {
    BonminCbcFullNodeInfo * nlpRoot = dynamic_cast<BonminCbcFullNodeInfo *> (parent);
    if(nlpRoot) {
      numberInfeasible = nlpRoot->getSequenceOfInfeasiblesSize();
      numberUnsolved =  nlpRoot->getSequenceOfUnsolvedSize();
    }
  }
  if((optimization_status==Ipopt::Unrecoverable_Exception)||
      (optimization_status==Ipopt::NonIpopt_Exception_Thrown)||
      (optimization_status==Ipopt::Insufficient_Memory)||
      (optimization_status==Ipopt::Restoration_Failed)||
      (optimization_status==Ipopt::Internal_Error)||
      (optimization_status==Ipopt::Maximum_Iterations_Exceeded))
    sequenceOfUnsolvedSize_ = numberUnsolved + 1;

  if(optimization_status==Ipopt::Infeasible_Problem_Detected)
    sequenceOfInfeasiblesSize_ = numberInfeasible + 1;
}

BonminCbcPartialNodeInfo::BonminCbcPartialNodeInfo (const BonminCbcPartialNodeInfo & rhs)

    : CbcPartialNodeInfo(rhs),
    sequenceOfInfeasiblesSize_(rhs.sequenceOfInfeasiblesSize_),
    sequenceOfUnsolvedSize_(rhs.sequenceOfUnsolvedSize_)

{}

CbcNodeInfo *
BonminCbcPartialNodeInfo::clone() const
{
  return (new BonminCbcPartialNodeInfo(*this));
}

void
BonminCbcPartialNodeInfo::allBranchesGone()
{
  IpoptWarmStartDiff * ipws = dynamic_cast<IpoptWarmStartDiff *>(basisDiff_);
  if(ipws)
    ipws->flushPoint();
}

BonminCbcPartialNodeInfo::~BonminCbcPartialNodeInfo ()
{}
