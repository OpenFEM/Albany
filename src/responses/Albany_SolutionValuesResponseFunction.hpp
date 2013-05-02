//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#ifndef ALBANY_SOLUTIONVALUESRESPONSEFUNCTION_HPP
#define ALBANY_SOLUTIONVALUESRESPONSEFUNCTION_HPP

#include "Albany_SamplingBasedScalarResponseFunction.hpp"

#include "Albany_Application.hpp"

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RCP.hpp"

class Epetra_Import;

namespace Albany {

  class SolutionCullingStrategyBase;

  /*!
   * \brief Reponse function representing the average of the solution values
   */
  class SolutionValuesResponseFunction :
    public SamplingBasedScalarResponseFunction {
  public:

    //! Constructor
    SolutionValuesResponseFunction(
      const Teuchos::RCP<const Application>& app,
      Teuchos::ParameterList& responseParams);

    //! Get the number of responses
    virtual unsigned int numResponses() const;

    //! Setup response function
    virtual void setup();

    //! Evaluate responses
    virtual void
    evaluateResponse(const double current_time,
		     const Epetra_Vector* xdot,
		     const Epetra_Vector& x,
		     const Teuchos::Array<ParamVec>& p,
		     Epetra_Vector& g);

    //! Evaluate tangent = dg/dx*dx/dp + dg/dxdot*dxdot/dp + dg/dp
    virtual void
    evaluateTangent(const double alpha,
		    const double beta,
		    const double current_time,
		    bool sum_derivs,
		    const Epetra_Vector* xdot,
		    const Epetra_Vector& x,
		    const Teuchos::Array<ParamVec>& p,
		    ParamVec* deriv_p,
		    const Epetra_MultiVector* Vxdot,
		    const Epetra_MultiVector* Vx,
		    const Epetra_MultiVector* Vp,
		    Epetra_Vector* g,
		    Epetra_MultiVector* gx,
		    Epetra_MultiVector* gp);

    //! Evaluate gradient = dg/dx, dg/dxdot, dg/dp
    virtual void
    evaluateGradient(const double current_time,
		     const Epetra_Vector* xdot,
		     const Epetra_Vector& x,
		     const Teuchos::Array<ParamVec>& p,
		     ParamVec* deriv_p,
		     Epetra_Vector* g,
		     Epetra_MultiVector* dg_dx,
		     Epetra_MultiVector* dg_dxdot,
		     Epetra_MultiVector* dg_dp);

  private:
    //! Private to prohibit copying
    SolutionValuesResponseFunction(const SolutionValuesResponseFunction&);

    //! Private to prohibit copying
    SolutionValuesResponseFunction& operator=(const SolutionValuesResponseFunction&);

    Teuchos::RCP<const Application> app_;

    Teuchos::RCP<SolutionCullingStrategyBase> cullingStrategy_;
    Teuchos::RCP<Epetra_Import> solutionImporter_;

    void updateSolutionImporter();
  };

}

#endif // ALBANY_SOLUTIONVALUESRESPONSEFUNCTION_HPP
