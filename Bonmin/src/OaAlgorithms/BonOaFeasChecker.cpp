// (C) Copyright International Business Machines 2006
// All Rights Reserved.
// This code is published under the Common Public License.
//
// Authors :
// P. Bonami, Carnegie Mellon University
//
// Date : 12/26/2006
//#define OA_DEBUG

#include "BonOaFeasChecker.hpp"
#include "BonminConfig.h"

#include "OsiAuxInfo.hpp"



namespace Bonmin
{

extern int usingCouenne;
// Default constructor
  OaFeasibilityChecker ::OaFeasibilityChecker ():
      OaDecompositionBase()
   {
   }



  OaFeasibilityChecker ::OaFeasibilityChecker 
  (OsiTMINLPInterface * nlp,
   OsiSolverInterface * si,
   double cbcCutoffIncrement,
   double cbcIntegerTolerance,
   bool leaveSiUnchanged
   )
      :
      OaDecompositionBase(nlp,si,
                          NULL, cbcCutoffIncrement,
                          cbcIntegerTolerance, leaveSiUnchanged)
  {
  }

  OaFeasibilityChecker ::~OaFeasibilityChecker ()
  {
  }

  /// OaDecomposition method 
  double
  OaFeasibilityChecker::performOa(OsiCuts & cs, solverManip &nlpManip, solverManip &lpManip, 
                  SubMipSolver * &subMip, OsiBabSolver * babInfo, double &cutoff) const
  {
   bool isInteger = true;
   bool feasible = 1;

   OsiSolverInterface * lp = lpManip.si();
   int numcols = lp->getNumCols();
   int origNumcols = nlp_->getNumCols();
   double milpBound = -DBL_MAX;
   int numberPasses = 0;
   double * nlpSol = usingCouenne ? new double[numcols] : NULL;
   while (isInteger && feasible ) {
     numberPasses++;

   //setup the nlp
   int numberCutsBefore = cs.sizeRowCuts();

   //Fix the variable which have to be fixed, after having saved the bounds
   double * colsol = const_cast<double *>(lp->getColSolution());
#if 0
   for(int i = 0 ; i < numcols ; i++)
     {
       std::cout<<"x["<<i<<"] = "<<colsol[i]<<"\t";
     }
   lp->writeLp("toto");
#endif
   nlpManip.fixIntegers(colsol);


   //Now solve the NLP get the cuts, and intall them in the local LP

   if(solveNlp(babInfo, cutoff)){
     //nlp solved and feasible
     // Update the cutoff
     cutoff = nlp_->getObjValue() *(1 - parameters_.cbcCutoffIncrement_);
     // Update the lp solver cutoff
     lp->setDblParam(OsiDualObjectiveLimit, cutoff);
   }
   // Get the cuts outer approximation at the current point

   if(usingCouenne){//Store solution and restore bounds because Couenne gives local cuts
     CoinCopyN(nlp_->getColSolution(), numcols, nlpSol);
     nlpManip.restore();}
   else{
     nlpSol = const_cast<double *>(nlp_->getColSolution());}
   
   const double * toCut = (parameter().addOnlyViolated_)?
			      colsol:NULL;
      nlp_->getOuterApproximation(cs, nlpSol, 1, toCut,
				  parameter().global_);
      int numberCuts = cs.sizeRowCuts() - numberCutsBefore;
      if (numberCuts > 0)
        lpManip.installCuts(cs, numberCuts);

        lp->resolve();
        double objvalue = lp->getObjValue();
        //milpBound = max(milpBound, lp->getObjValue());
        feasible = (lp->isProvenOptimal() &&
            !lp->isDualObjectiveLimitReached() && (objvalue<cutoff)) ;
        //if value of integers are unchanged then we have to get out
        bool changed = !feasible;//if lp is infeasible we don't have to check anything
	if(!changed){
	  if(!usingCouenne)
	    changed = nlpManip.isDifferentOnIntegers(lp->getColSolution());
	}
          if (changed) {
            isInteger = integerFeasible(lp->getColSolution(), origNumcols);
          }
          else {
            isInteger = 0;
            //	  if(!fixed)//fathom on bounds
            milpBound = 1e200;
          }
#ifdef OA_DEBUG
          printf("Obj value after cuts %g %d rows\n",lp->getObjValue(),
              numberCuts) ;
#endif
      }
#ifdef OA_DEBUG
    debug_.printEndOfProcedureDebugMessage(cs, foundSolution, milpBound, isInteger, feasible, std::cout);
#endif
   std::cout<<"milpBound found: "<<milpBound<<std::endl;
   if(usingCouenne)
     delete [] nlpSol;
    return milpBound;
  }

}/* End namespace Bonmin. */
