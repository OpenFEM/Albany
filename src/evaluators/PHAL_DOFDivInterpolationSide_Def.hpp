//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include "Teuchos_TestForException.hpp"
#include "Phalanx_DataLayout.hpp"

#include "Intrepid_FunctionSpaceTools.hpp"

namespace PHAL {

//**********************************************************************
template<typename EvalT, typename Traits>
DOFDivInterpolationSide<EvalT, Traits>::
DOFDivInterpolationSide(const Teuchos::ParameterList& p,
                         const Teuchos::RCP<Albany::Layouts>& dl) :
  sideSetName (p.get<std::string> ("Side Set Name")),
  val_node    (p.get<std::string> ("Variable Name"), dl->side_node_vector),
  gradBF      (p.get<std::string> ("Gradient BF Name"), dl->side_node_qp_gradient),
  val_qp      (p.get<std::string> ("Divergence Variable Name"), dl->side_qp_scalar )
{
  this->addDependentField(val_node);
  this->addDependentField(gradBF);
  this->addEvaluatedField(val_qp);

  this->setName("DOFDivInterpolationSide" );

  std::vector<PHX::DataLayout::size_type> dims;
  gradBF.fieldTag().dataLayout().dimensions(dims);
  numSideNodes = dims[2];
  numSideQPs   = dims[3];
  numDims      = dims[4];
}

//**********************************************************************
template<typename EvalT, typename Traits>
void DOFDivInterpolationSide<EvalT, Traits>::
postRegistrationSetup(typename Traits::SetupData d,
                      PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(val_node,fm);
  this->utils.setFieldData(gradBF,fm);
  this->utils.setFieldData(val_qp,fm);
}

//**********************************************************************
template<typename EvalT, typename Traits>
void DOFDivInterpolationSide<EvalT, Traits>::
evaluateFields(typename Traits::EvalData workset)
{
  if (workset.sideSets->find(sideSetName)==workset.sideSets->end())
    return;

  const std::vector<Albany::SideStruct>& sideSet = workset.sideSets->at(sideSetName);
  for (auto const& it_side : sideSet)
  {
    // Get the local data of side and cell
    const int cell = it_side.elem_LID;
    const int side = it_side.side_local_id;

    for (int qp=0; qp<numSideQPs; ++qp)
    {
      val_qp(cell,side,qp) = 0.;
      for (int dim=0; dim<numDims; ++dim)
      {
        for (int node=0; node<numSideNodes; ++node)
        {
          val_qp(cell,side,qp) += val_node(cell,side,node,dim) * gradBF(cell,side,node,qp,dim);
        }
      }
    }
  }
}

} // Namespace PHAL