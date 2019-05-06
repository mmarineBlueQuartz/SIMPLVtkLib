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

#include "VSMaskFilter.h"

#include <QtCore/QString>
#include <QtCore/QUuid>

#include <vtkCellData.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter::VSMaskFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_MaskAlgorithm = nullptr;
  setParentFilter(parent);

  m_MaskValues = new VSMaskValues(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter::VSMaskFilter(const VSMaskFilter& copy)
: VSAbstractFilter()
{
  m_MaskAlgorithm = nullptr;
  setParentFilter(copy.getParentFilter());

  m_MaskValues = new VSMaskValues(*(copy.m_MaskValues));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter* VSMaskFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  VSMaskFilter* filter = new VSMaskFilter(parent);

  filter->m_MaskValues->setLastArrayName(json["Last Array Name"].toString());

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::createFilter()
{
  m_MaskAlgorithm = VTK_PTR(vtkThreshold)::New();
  m_MaskAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskFilter::getFilterName() const
{
  return "Mask";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskFilter::getToolTip() const
{
  return "Mask Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSMaskFilter::getFilterType() const
{
  return FilterType::Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::applyValues(VSMaskValues* values)
{
  if(values)
  {
    apply(values->getMaskName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::apply(QString name)
{
  if(nullptr == m_MaskAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Mask-Type widgets
  m_MaskValues->setLastArrayName(name);

  m_MaskAlgorithm->ThresholdByUpper(1.0);
  m_MaskAlgorithm->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, qPrintable(name));
  m_MaskAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::readJson(QJsonObject& json)
{
  m_MaskValues->loadJson(json);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);
  m_MaskValues->writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSMaskFilter::getOutputPort()
{
  if(getConnectedInput() && m_MaskAlgorithm)
  {
    return m_MaskAlgorithm->GetOutputPort();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutputPort();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSMaskFilter::getOutput() const
{
  if(getConnectedInput() && m_MaskAlgorithm)
  {
    return m_MaskAlgorithm->GetOutput();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutput();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_MaskAlgorithm)
  {
    m_MaskAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSMaskFilter::GetUuid()
{
  return QUuid("{baedbc9c-3c2c-5428-a4b2-1ea06ace5aba}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSMaskFilter::getOutputType() const
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSMaskFilter::GetRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMaskFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  // Require scalar values
  vtkDataSet* output = filter->getOutput();
  if(output && output->GetCellData() && output->GetCellData()->GetScalars())
  {
    if(CompatibleInput(filter->getOutputType(), GetRequiredInputType()))
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMaskFilter::CompatibleWithParents(VSAbstractFilter::FilterListType filters)
{
  if(filters.size() == 0)
  {
    return false;
  }

  for(VSAbstractFilter* filter : filters)
  {
    if(false == CompatibleWithParent(filter))
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterValues* VSMaskFilter::getValues()
{
  return m_MaskValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSMaskFilter::getInfoString(SIMPL::InfoStringFormat format) const
{
  return QString();
}
