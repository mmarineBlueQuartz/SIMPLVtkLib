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

#include "VSClipFilter.h"

#include <QApplication>

#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QUuid>

#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipValues.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(parent);

  m_ClipValues = new VSClipValues(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(const VSClipFilter& copy)
: VSAbstractFilter()
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(copy.getParentFilter());

  m_ClipValues = new VSClipValues(*copy.m_ClipValues);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter* VSClipFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  VSClipFilter* filter = new VSClipFilter(parent);
  filter->m_ClipValues->loadJSon(json);

  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::createFilter()
{
  m_ClipAlgorithm = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
  m_ClipAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSClipFilter::getFilterName() const
{
  return "Clip";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSClipFilter::getToolTip() const
{
  return "Clip Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSClipFilter::getFilterType() const
{
  return FilterType::Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipValues* VSClipFilter::getValues() const
{
  return m_ClipValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::applyValues(VSClipValues* values)
{
  // Copy these values if they are not the ones owned by this filter
  bool ownedValues = (values == m_ClipValues);

  switch(values->getClipType())
  {
  case ClipType::BOX:
  {
    VSBoxWidget* boxWidget = values->getBoxWidget();
    apply(boxWidget->getPlanes(), boxWidget->getTransform(), values->isInverted());

    if(!ownedValues)
    {
      m_ClipValues->getBoxWidget()->setTransform(boxWidget->getTransform());
      m_ClipValues->setInverted(values->isInverted());
    }
    break;
  }
  case ClipType::PLANE:
  {
    VSPlaneWidget* planeWidget = values->getPlaneWidget();
    double origin[3];
    double normal[3];
    planeWidget->getOrigin(origin);
    planeWidget->getNormal(normal);
    apply(origin, normal, values->isInverted());

    if(!ownedValues)
    {
      m_ClipValues->setClipType(values->getClipType());
      m_ClipValues->getPlaneWidget()->setOrigin(origin);
      m_ClipValues->getPlaneWidget()->setNormal(normal);
      m_ClipValues->setInverted(values->isInverted());
    }
    break;
  }
  default:
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(double origin[3], double normal[3], bool inverted)
{
  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Plane-Type clips
  m_ClipValues->setLastClipType(ClipType::PLANE);
  m_ClipValues->setLastPlaneInverted(inverted);

  // Save the applied values for resetting Plane-Type widgets
  m_ClipValues->setLastPlaneOrigin(origin);
  m_ClipValues->setLastPlaneNormal(normal);

  VTK_NEW(vtkPlane, plane);
  plane->SetOrigin(origin);
  plane->SetNormal(normal);

  m_ClipAlgorithm->SetClipFunction(plane);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
  emit clipTypeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(VTK_PTR(vtkPlanes) planes, VTK_PTR(vtkTransform) transform, bool inverted)
{
  if(nullptr == planes)
  {
    return;
  }

  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Box-Type clips
  m_ClipValues->setLastClipType(ClipType::BOX);
  m_ClipValues->setLastBoxInverted(inverted);
  m_ClipValues->setLastBoxTransform(transform);

  m_ClipAlgorithm->SetClipFunction(planes);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
  emit clipTypeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(std::vector<double> originVector, std::vector<double> normalVector, bool inverted)
{
  if(originVector.size() != 3 || normalVector.size() != 3)
  {
    return;
  }

  double* origin = new double[3];
  double* normal = new double[3];
  for(int i = 0; i < 3; i++)
  {
    origin[i] = originVector[i];
    normal[i] = normalVector[i];
  }

  apply(origin, normal, inverted);

  delete[] origin;
  delete[] normal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply(std::vector<double> origin, std::vector<double> rotation, std::vector<double> scale, bool inverted)
{
  qDebug() << "Implement Box Clip from Transform";

  VSTransform* transform = new VSTransform();
  transform->setLocalPositionVector(origin);
  transform->setLocalRotationVector(rotation);
  transform->setLocalScaleVector(scale);

  VTK_NEW(vtkPlanes, planes);
  VTK_NEW(vtkPoints, points);
  VTK_NEW(vtkDoubleArray, normals);

  points->SetNumberOfPoints(6);
  normals->SetNumberOfComponents(3);
  normals->SetNumberOfTuples(6);

  // Front
  {
    double normal[3]{0.0, 0.0, 1.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{0.0, 0.0, 1.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(0, i, normal[i]);
    }
    points->SetPoint(0, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  // Back
  {
    double normal[3]{0.0, 0.0, -1.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{0.0, 0.0, -1.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(1, i, normal[i]);
    }
    points->SetPoint(1, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  // Left
  {
    double normal[3]{-1.0, 0.0, 0.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{-1.0, 0.0, 0.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(2, i, normal[i]);
    }
    points->SetPoint(2, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  // Right
  {
    double normal[3]{1.0, 0.0, 0.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{1.0, 0.0, 0.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(3, i, normal[i]);
    }
    points->SetPoint(3, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  // Top
  {
    double normal[3]{0.0, 1.0, 0.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{0.0, 1.0, 0.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(4, i, normal[i]);
    }
    points->SetPoint(4, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  // Bottom
  {
    double normal[3]{0.0, -1.0, 0.0};
    transform->globalizeNormal(normal);

    double planeOrigin[3]{0.0, -1.0, 0.0};
    transform->globalizePoint(planeOrigin);

    for(int i = 0; i < 3; i++)
    {
      normals->SetComponent(5, i, normal[i]);
    }
    points->SetPoint(5, planeOrigin);

    qDebug() << "Normals: " << normal[0] << ", " << normal[1] << ", " << normal[2];
    qDebug() << "Points: " << planeOrigin[0] << ", " << planeOrigin[1] << ", " << planeOrigin[2];
  }

  planes->SetNormals(normals);
  planes->SetPoints(points);

  // Apply box clip
  apply(planes, transform->getGlobalTransform(), inverted);

  delete transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSClipFilter::GetUuid()
{
  return QUuid("{be4f6ccb-d4eb-56b3-bddc-94dd5056fdd2}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSClipFilter::getOutputPort()
{
  if(getConnectedInput() && m_ClipAlgorithm)
  {
    return m_ClipAlgorithm->GetOutputPort();
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
VTK_PTR(vtkDataSet) VSClipFilter::getOutput() const
{
  if(getConnectedInput() && m_ClipAlgorithm)
  {
    return m_ClipAlgorithm->GetOutput();
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
void VSClipFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_ClipAlgorithm)
  {
    m_ClipAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);
  m_ClipValues->writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::getOutputType() const
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::GetRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  dataType_t outputType = filter->getOutputType();
  dataType_t requiredType = GetRequiredInputType();
  if(CompatibleInput(outputType, requiredType))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::CompatibleWithParents(VSAbstractFilter::FilterListType filters)
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
VSAbstractFilterValues* VSClipFilter::getValues()
{
  return m_ClipValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSClipFilter::getClipTypes()
{
  QStringList clipTypes;
  clipTypes.push_back("Plane");
  clipTypes.push_back("Box");

  return clipTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSClipFilter::getInfoString(SIMPL::InfoStringFormat format) const
{
  return QString();
}
