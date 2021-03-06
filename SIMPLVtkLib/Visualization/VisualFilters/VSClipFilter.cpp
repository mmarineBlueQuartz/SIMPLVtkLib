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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(parent);

  for(int i = 0; i < 3; i++)
  {
    m_LastPlaneOrigin[i] = 0.0;
    m_LastPlaneNormal[i] = 0.0;
  }

  m_LastBoxTransform = VTK_PTR(vtkTransform)::New();

  // Set the direction of the plane normal
  m_LastPlaneNormal[0] = 1.0;

  m_LastClipType = VSClipFilter::ClipType::PLANE;
  m_LastPlaneInverted = false;
  m_LastBoxInverted = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(const VSClipFilter& copy)
: VSAbstractFilter()
, m_LastClipType(copy.m_LastClipType)
, m_LastPlaneInverted(copy.m_LastPlaneInverted)
, m_LastBoxInverted(copy.m_LastBoxInverted)
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(copy.getParentFilter());

  for(int i = 0; i < 3; i++)
  {
    m_LastPlaneOrigin[i] = copy.m_LastPlaneOrigin[i];
    m_LastPlaneNormal[i] = copy.m_LastPlaneNormal[i];
  }

  m_LastBoxTransform = VTK_PTR(vtkTransform)::New();
  m_LastBoxTransform->DeepCopy(copy.m_LastBoxTransform);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter* VSClipFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  VSClipFilter* filter = new VSClipFilter(parent);

  filter->setLastClipType(static_cast<ClipType>(json["Last Clip Type"].toInt()));
  filter->setLastPlaneInverted(json["Last Plane Inverted"].toBool());
  filter->setLastBoxInverted(json["Last Box Inverted"].toBool());

  QJsonArray lastPlaneOrigin = json["Last Plane Origin"].toArray();
  double origin[3];
  origin[0] = lastPlaneOrigin.at(0).toDouble();
  origin[1] = lastPlaneOrigin.at(1).toDouble();
  origin[2] = lastPlaneOrigin.at(2).toDouble();
  filter->setLastPlaneOrigin(origin);

  QJsonArray lastPlaneNormal = json["Last Plane Normal"].toArray();
  double normals[3];
  normals[0] = lastPlaneNormal.at(0).toDouble();
  normals[1] = lastPlaneNormal.at(1).toDouble();
  normals[2] = lastPlaneNormal.at(2).toDouble();
  filter->setLastPlaneNormal(normals);

  QJsonArray boxTransformDataArray = json["Box Transform Data"].toArray();
  double boxTransformData[16];
  for(int i = 0; i < 16; i++)
  {
    boxTransformData[i] = boxTransformDataArray[i].toDouble();
  }

  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  matrix->DeepCopy(boxTransformData);

  VTK_NEW(vtkTransform, transform);
  transform->SetMatrix(matrix);

  filter->setLastBoxTransform(transform);

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
void VSClipFilter::apply(double origin[3], double normal[3], bool inverted)
{
  if(nullptr == m_ClipAlgorithm)
  {
    createFilter();
  }

  // Handle Plane-Type clips
  m_LastClipType = ClipType::PLANE;
  m_LastPlaneInverted = inverted;

  // Save the applied values for resetting Plane-Type widgets
  for(int i = 0; i < 3; i++)
  {
    m_LastPlaneOrigin[i] = origin[i];
    m_LastPlaneNormal[i] = normal[i];
  }

  VTK_NEW(vtkPlane, plane);
  plane->SetOrigin(origin);
  plane->SetNormal(normal);

  m_ClipAlgorithm->SetClipFunction(plane);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
  emit clipTypeChanged();
  emit lastPlaneOriginChanged();
  emit lastPlaneNormalChanged();
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
  m_LastClipType = ClipType::BOX;
  m_LastBoxInverted = inverted;
  m_LastBoxTransform->DeepCopy(transform);

  m_ClipAlgorithm->SetClipFunction(planes);
  m_ClipAlgorithm->SetInsideOut(inverted);
  m_ClipAlgorithm->Update();

  emit updatedOutputPort(this);
  emit clipTypeChanged();
  emit lastBoxTranslationChanged();
  emit lastBoxRotationChanged();
  emit lastBoxScaleChanged();
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

  json["Last Clip Type"] = static_cast<int>(m_LastClipType);
  json["Last Plane Inverted"] = m_LastPlaneInverted;
  json["Last Box Inverted"] = m_LastBoxInverted;

  QJsonArray lastPlaneOrigin;
  lastPlaneOrigin.append(m_LastPlaneOrigin[0]);
  lastPlaneOrigin.append(m_LastPlaneOrigin[1]);
  lastPlaneOrigin.append(m_LastPlaneOrigin[2]);
  json["Last Plane Origin"] = lastPlaneOrigin;

  QJsonArray lastPlaneNormal;
  lastPlaneNormal.append(m_LastPlaneNormal[0]);
  lastPlaneNormal.append(m_LastPlaneNormal[1]);
  lastPlaneNormal.append(m_LastPlaneNormal[2]);
  json["Last Plane Normal"] = lastPlaneNormal;

  json["Uuid"] = GetUuid().toString();

  vtkMatrix4x4* matrix = m_LastBoxTransform->GetMatrix();
  double* matrixData = matrix->GetData();
  QJsonArray boxTransformData;
  for(int i = 0; i < 16; i++)
  {
    boxTransformData.append(matrixData[i]);
  }

  json["Box Transform Data"] = boxTransformData;
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
VSAbstractFilter::dataType_t VSClipFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::compatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  dataType_t outputType = filter->getOutputType();
  dataType_t requiredType = getRequiredInputType();
  if(compatibleInput(outputType, requiredType))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::getLastPlaneInverted() const
{
  return m_LastPlaneInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastPlaneOrigin()
{
  return m_LastPlaneOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipFilter::getLastPlaneNormal()
{
  return m_LastPlaneNormal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipFilter::getLastPlaneOriginVector() const
{
  std::vector<double> origin(3);
  for(int i = 0; i < 3; i++)
  {
    origin[i] = m_LastPlaneOrigin[i];
  }

  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipFilter::getLastPlaneNormalVector() const
{
  std::vector<double> normal(3);
  for(int i = 0; i < 3; i++)
  {
    normal[i] = m_LastPlaneNormal[i];
  }

  return normal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipFilter::getLastBoxInverted() const
{
  return m_LastBoxInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSClipFilter::getLastBoxTransform()
{
  return m_LastBoxTransform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipFilter::getLastBoxTranslationVector() const
{
  std::vector<double> origin(3);
  double* position = new double[3];
  m_LastBoxTransform->GetPosition(position);
  for(int i = 0; i < 3; i++)
  {
    origin[i] = position[i];
  }

  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipFilter::getLastBoxRotationVector() const
{
  std::vector<double> rotation(3);
  double* rotationPtr = new double[3];
  m_LastBoxTransform->GetOrientation(rotationPtr);
  for(int i = 0; i < 3; i++)
  {
    rotation[i] = rotationPtr[i];
  }

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipFilter::getLastBoxScaleVector() const
{
  std::vector<double> scale(3);
  double* scalePtr = new double[3];
  m_LastBoxTransform->GetOrientation(scalePtr);
  for(int i = 0; i < 3; i++)
  {
    scale[i] = scalePtr[i];
  }

  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::ClipType VSClipFilter::getLastClipType()
{
  return m_LastClipType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastPlaneInverted(bool inverted)
{
  m_LastPlaneInverted = inverted;
  emit lastPlaneInvertedChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastPlaneOrigin(double* origin)
{
  m_LastPlaneOrigin[0] = origin[0];
  m_LastPlaneOrigin[1] = origin[1];
  m_LastPlaneOrigin[2] = origin[2];

  emit lastPlaneOriginChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastPlaneNormal(double* normal)
{
  m_LastPlaneNormal[0] = normal[0];
  m_LastPlaneNormal[1] = normal[1];
  m_LastPlaneNormal[2] = normal[2];

  emit lastPlaneNormalChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastBoxInverted(bool inverted)
{
  m_LastBoxInverted = inverted;
  emit lastBoxInvertedChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastBoxTransform(VTK_PTR(vtkTransform) transform)
{
  m_LastBoxTransform = transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setLastClipType(VSClipFilter::ClipType type)
{
  m_LastClipType = type;
  emit clipTypeChanged();
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
