/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VSDataSetFilter.h"

#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkTrivialProducer.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter::VSDataSetFilter(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDataContainer)
: VSAbstractFilter()
, m_WrappedDataContainer(wrappedDataContainer)
{
  setFilter();

  setText(wrappedDataContainer->m_Name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter::~VSDataSetFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSDataSetFilter::getBounds()
{
  return m_WrappedDataContainer->m_DataSet->GetBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::setBounds(double* bounds)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSDataSetFilter::setInputData(VTK_PTR(vtkDataSet) inputData)
//{
//  m_dataSet = inputData;
//  m_dataSet->ComputeBounds();
//
//  if(m_trivialProducer != nullptr)
//  {
//    m_trivialProducer->SetOutput(m_dataSet);
//  }
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDataSetFilter::setFilter()
{
  VTK_PTR(vtkDataSet) dataSet = m_WrappedDataContainer->m_DataSet;
  dataSet->ComputeBounds();
  
  vtkCellData* cellData = dataSet->GetCellData();
  if(cellData)
  {
    vtkDataArray* dataArray = cellData->GetArray(0);
    if(dataArray)
    {
      char* name = dataArray->GetName();
      cellData->SetActiveScalars(name);
    }
    
  }
  
  m_TrivialProducer = VTK_PTR(vtkTrivialProducer)::New();
  m_TrivialProducer->SetOutput(dataSet);

  m_OutputProducer->SetInputConnection(m_TrivialProducer->GetOutputPort());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSDataSetFilter::getFilterName()
{
  return m_WrappedDataContainer->m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSDataSetFilter::getWrappedDataContainer()
{
  return m_WrappedDataContainer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSDataSetFilter::getOutputType()
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSDataSetFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}