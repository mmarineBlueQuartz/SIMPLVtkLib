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

#include "VSCropFilter.h"

#include <QtCore/QString>

#include <QtCore/QJsonArray>
#include <QtCore/QUuid>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include <vtkExtractVOI.h>

#include <vtkRenderWindowInteractor.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropFilter::VSCropFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_CropAlgorithm = nullptr;
  setParentFilter(parent);

  m_CropValues = new VSCropValues(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropFilter::VSCropFilter(const VSCropFilter& copy)
: VSAbstractFilter()
{
  m_CropAlgorithm = nullptr;
  setParentFilter(copy.getParentFilter());

  m_CropValues = new VSCropValues(*(copy.m_CropValues));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSCropFilter* VSCropFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  VSCropFilter* filter = new VSCropFilter(parent);
  filter->m_CropValues->loadJson(json);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::createFilter()
{
  m_CropAlgorithm = vtkSmartPointer<vtkExtractVOI>::New();
  m_CropAlgorithm->IncludeBoundaryOn();
  m_CropAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());

  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::applyValues(VSCropValues* values)
{
  if(values)
  {
    apply(values->getVOI(), values->getSampleRate());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::apply(int voi[6], int sampleRate[3])
{
  if(nullptr == m_CropAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Crop-Type widgets
  m_CropAlgorithm->SetVOI(voi);
  m_CropAlgorithm->SetSampleRate(sampleRate);
  m_CropAlgorithm->Update();

  m_CropValues->setLastVOI(voi);
  m_CropValues->setLastSampleRate(sampleRate);

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::apply(std::vector<int> voiVector, std::vector<int> sampleRateVector)
{
  if(voiVector.size() != 6 || sampleRateVector.size() != 3)
  {
    qDebug() << "VSCropFilter::apply() called with invalid vector sizes";
    qDebug() << "  VOI size = " << voiVector.size() << " Sample Rate size = " << sampleRateVector.size();
    return;
  }

  int voi[6];
  for(int i = 0; i < 6; i++)
  {
    voi[i] = voiVector[i];
  }

  int sampleRate[3];
  for(int i = 0; i < 3; i++)
  {
    sampleRate[i] = sampleRateVector[i];
  }

  apply(voi, sampleRate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSCropFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);
  m_CropValues->writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSCropFilter::GetUuid()
{
  return QUuid("{65a3f063-5054-5abe-b518-3a2884d96b1c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSCropFilter::getFilterName() const
{
  return "Crop";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSCropFilter::getToolTip() const
{
  return "Crop Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSCropFilter::getFilterType() const
{
  return FilterType::Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSCropFilter::getOutputPort()
{
  if(getConnectedInput() && m_CropAlgorithm)
  {
    return m_CropAlgorithm->GetOutputPort();
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
VTK_PTR(vtkDataSet) VSCropFilter::getOutput() const
{
  if(getConnectedInput() && m_CropAlgorithm)
  {
    return m_CropAlgorithm->GetOutput();
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
void VSCropFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_CropAlgorithm)
  {
    m_CropAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSCropFilter::getOutputType() const
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSCropFilter::GetRequiredInputType()
{
  return IMAGE_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSCropFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(CompatibleInput(filter->getOutputType(), GetRequiredInputType()))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSCropFilter::CompatibleWithParents(VSAbstractFilter::FilterListType filters)
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
VSAbstractFilterValues* VSCropFilter::getValues()
{
  return m_CropValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSCropFilter::getInfoString(SIMPL::InfoStringFormat format) const
{
  return QString();
}
