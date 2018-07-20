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

#include "VSAbstractFilter.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <vtkAlgorithm.h>
#include <vtkCellData.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/SIMPLBridge/VSVertexGeom.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::VSAbstractFilter()
: QObject()
, m_LoadingObject(QJsonObject())
, m_Transform(new VSTransform())
, m_ChildLock(1)
, m_InputPort(nullptr)
{
  setCheckable(true);
  setChecked(true);

  m_Flags = Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;

  QThread* thread = QCoreApplication::instance()->thread();
  m_Transform->moveToThread(thread);

  connect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), this, SLOT(connectAdditionalOutputFilters(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::deleteFilter()
{
  emit removeFilter();

  if(getParentFilter())
  {
    getParentFilter()->removeChild(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags VSAbstractFilter::flags() const
{
  return m_Flags;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getParentFilter() const
{
  QObject* parentObj = parent();
  VSAbstractFilter* parentFilter = dynamic_cast<VSAbstractFilter*>(parentObj);
  return parentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setParentFilter(VSAbstractFilter* parent)
{
  if(getParentFilter())
  {
    disconnect(parent, SIGNAL(updatedOutput()), this, SIGNAL(updatedOutput()));
  }

  QObject::setParent(parent);
  if(parent)
  {
    parent->addChild(this);

    // Sets the transform's parent as well
    m_Transform->setParent(parent->getTransform());
    setInputPort(parent->getOutputPort());

    connect(parent, SIGNAL(updatedOutput()), this, SIGNAL(updatedOutput()));
  }
  else
  {
    // Sets the transform's parent to nullptr
    m_Transform->setParent(nullptr);
  }

  updateTransformFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getChildIndex() const
{
  if(nullptr == getParentFilter())
  {
    return 0;
  }

  return getParentFilter()->getIndexOfChild(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::addChild(VSAbstractFilter* child)
{
  connect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), child, SLOT(connectToOutput(VSAbstractFilter*)), Qt::UniqueConnection);

  // Avoid crashing when adding children from multiple threads
  m_ChildLock.acquire();
  VSFilterModel* model = getModel();
  if(model)
  {
    model->beginInsertingFilter(this);
    m_Children.push_back(child);
    model->endInsertingFilter(child);
  }
  m_ChildLock.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::removeChild(VSAbstractFilter* child)
{
  int row = getIndexOfChild(child);

  disconnect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), child, SLOT(connectToOutput(VSAbstractFilter*)));
  m_ChildLock.acquire();
  VSFilterModel* model = getModel();
  if(model)
  {
    model->beginRemovingFilter(this, row);
    m_Children.remove(child);
    model->endRemovingFilter(child);
  }
  m_ChildLock.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getAncestor()
{
  if(nullptr == getParentFilter())
  {
    return this;
  }

  return getParentFilter()->getAncestor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSAbstractFilter::getChildren() const
{
  return m_Children;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getChildCount() const
{
  return m_Children.size();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getIndexOfChild(const VSAbstractFilter* childFilter) const
{
  if(nullptr == childFilter)
  {
    return -1;
  }

  m_ChildLock.acquire();
  int i = 0;
  for(auto iter = m_Children.begin(); iter != m_Children.end(); iter++)
  {
    if(childFilter == (*iter))
    {
      m_ChildLock.release();
      return i;
    }
    i++;
  }
  m_ChildLock.release();

  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSAbstractFilter::getDescendants() const
{
  FilterListType descendants;
  FilterListType children = getChildren();

  for(VSAbstractFilter* filter : children)
  {
    descendants.push_back(filter);
    FilterListType filterDescendants = filter->getDescendants();

    descendants.insert(descendants.end(), filterDescendants.begin(), filterDescendants.end());
  }

  return descendants;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getChild(int index) const
{
  if(index < 0 || index >= getChildCount())
  {
    return nullptr;
  }

  auto iter = m_Children.begin();
  for(int i = 0; i < index; i++)
  {
    iter++;
  }

  return (*iter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::isPointData()
{
  // Check for output types
  if(VSAbstractFilter::dataType_t::POINT_DATA == getOutputType())
  {
    return true;
  }

  // Check for class types
  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(nullptr == dataSet)
  {
    return false;
  }

  if(VSVertexGrid::SafeDownCast(dataSet))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getArrayNames()
{
  QStringList arrayNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet)
  {
    if(isPointData())
    {
      int numPointArrays = dataSet->GetPointData()->GetNumberOfArrays();
      for(int i = 0; i < numPointArrays; i++)
      {
        arrayNames.push_back(dataSet->GetPointData()->GetArrayName(i));
      }
    }
    else
    {
      int numCellArrays = dataSet->GetCellData()->GetNumberOfArrays();
      for(int i = 0; i < numCellArrays; i++)
      {
        arrayNames.push_back(dataSet->GetCellData()->GetArrayName(i));
      }
    }
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(QString arrayName)
{
  QStringList componentNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet && !arrayName.isNull())
  {
    VTK_PTR(vtkAbstractArray) array = nullptr;
    if(isPointData())
    {
      array = dataSet->GetPointData()->GetAbstractArray(qPrintable(arrayName));
    }
    else
    {
      array = dataSet->GetCellData()->GetAbstractArray(qPrintable(arrayName));
    }
    componentNames = getComponentList(array);
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(int arrayIndex)
{
  QStringList componentNames;

  VTK_PTR(vtkDataSet) dataSet = getOutput();
  if(dataSet)
  {
    VTK_PTR(vtkAbstractArray) array = nullptr;
    if(isPointData())
    {
      array = dataSet->GetPointData()->GetAbstractArray(arrayIndex);
    }
    else
    {
      array = dataSet->GetCellData()->GetAbstractArray(arrayIndex);
    }
    // Get component names from the array
    componentNames = getComponentList(array);
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList VSAbstractFilter::getComponentList(vtkAbstractArray* array)
{
  QStringList componentNames;

  if(array)
  {
    bool isCharArray = array->IsA("vtkUnsignedCharArray");
    bool isFloatArray = array->IsA("vtkFloatArray");
    int numComponent = array->GetNumberOfComponents();
    if(isCharArray && numComponent == 3)
    {
      componentNames.push_back("R");
      componentNames.push_back("G");
      componentNames.push_back("B");
    }
    else if(isFloatArray && numComponent == 3)
    {
      componentNames.push_back("X");
      componentNames.push_back("Y");
      componentNames.push_back("Z");
    }
    else
    {
      for(int i = 0; i < numComponent; i++)
      {
        componentNames.push_back("Comp " + QString::number(i + 1));
      }
    }

    if(numComponent > 1)
    {
      componentNames.push_front("Magnitude");
    }
  }

  return componentNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getBounds() const
{
  if(nullptr == getParentFilter())
  {
    return nullptr;
  }

  return getParentFilter()->getBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractDataFilter* VSAbstractFilter::getDataSetFilter()
{
  VSAbstractDataFilter* cast = dynamic_cast<VSAbstractDataFilter*>(this);

  if(cast != nullptr)
  {
    return cast;
  }

  if(nullptr == getParentFilter())
  {
    return nullptr;
  }

  return getParentFilter()->getDataSetFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSAbstractFilter::getOutputType()
{
  int dataType = getOutput()->GetDataObjectType();
  switch(dataType)
  {
  case VTK_IMAGE_DATA:
    return dataType_t::IMAGE_DATA;
  case VTK_STRUCTURED_GRID:
    return dataType_t::STRUCTURED_GRID;
  case VTK_RECTILINEAR_GRID:
    return dataType_t::RECTILINEAR_GRID;
  case VTK_STRUCTURED_POINTS:
    return dataType_t::POINT_DATA;
  case VTK_UNSTRUCTURED_GRID:
  case VTK_UNSTRUCTURED_GRID_BASE:
    return dataType_t::UNSTRUCTURED_GRID;
  case VTK_POLY_DATA:
    return dataType_t::POLY_DATA;
  default:
    return dataType_t::INVALID_DATA;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::compatibleInput(VSAbstractFilter::dataType_t inputType, VSAbstractFilter::dataType_t requiredType)
{
  if(inputType == INVALID_DATA)
  {
    return false;
  }

  if(requiredType == ANY_DATA_SET)
  {
    return true;
  }

  return requiredType == inputType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::saveFile(QString fileName)
{
  vtkDataSet* output = getOutput();

  VTK_NEW(vtkGenericDataObjectWriter, writer);

  writer->SetFileName(fileName.toStdString().c_str());
  writer->SetInputData(output);
  writer->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::getConnectedInput()
{
  return m_ConnectedInput;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setConnectedInput(bool connected)
{
  m_ConnectedInput = connected;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkAlgorithmOutput) VSAbstractFilter::getInputPort()
{
  return m_InputPort;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setInputPort(VTK_PTR(vtkAlgorithmOutput) inputPort)
{
  m_InputPort = inputPort;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::connectToOutput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getTransformedOutputPort());

  if(getConnectedInput())
  {
    // Connect algorithm input and filter output
    updateAlgorithmInput(filter);
  }
  else
  {
    // Emit only when the filter is not connected to its algorithm
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::connectAdditionalOutputFilters(VSAbstractFilter* filter)
{
  // Update the transform filter's input port if the filter exists
  if(m_TransformFilter)
  {
    m_TransformFilter->SetInputConnection(getOutputPort());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSAbstractFilter::getTransform()
{
  return m_Transform.get();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSAbstractFilter::getTransformedOutputPort()
{
  if(nullptr == m_TransformFilter)
  {
    createTransformFilter();
  }

  return m_TransformFilter->GetOutputPort();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSAbstractFilter::getTransformedOutput()
{
  if(nullptr == m_TransformFilter)
  {
    createTransformFilter();
  }

  return m_TransformFilter->GetOutput();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::createTransformFilter()
{
  m_TransformFilter = VTK_PTR(vtkTransformFilter)::New();

  if(m_Transform)
  {
    m_TransformFilter->SetTransform(m_Transform->getGlobalTransform());
    connect(m_Transform.get(), SIGNAL(valuesChanged()), this, SLOT(updateTransformFilter()));
  }
  else
  {
    VTK_NEW(vtkTransform, transform);
    m_TransformFilter->SetTransform(transform);
  }

  m_TransformFilter->SetInputConnection(getOutputPort());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransformFilter) VSAbstractFilter::getTransformFilter()
{
  if(nullptr == m_TransformFilter)
  {
    createTransformFilter();
  }

  return m_TransformFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::updateTransformFilter()
{
  if(nullptr == m_TransformFilter)
  {
    createTransformFilter();
  }

  if(getTransform())
  {
    m_TransformFilter->SetTransform(getTransform()->getGlobalTransform());
    emit transformChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getTransformBounds()
{
  if(nullptr == m_TransformFilter || nullptr == getParentFilter())
  {
    return getBounds();
  }

  vtkTransformFilter* trans = vtkTransformFilter::New();
  trans->SetInputConnection(getParentFilter()->getOutputPort());
  trans->SetTransform(getTransform()->getGlobalTransform());
  trans->Update();

  return trans->GetOutput()->GetBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::writeJson(QJsonObject& json)
{
  json["CheckState"] = checkState();

  // Save the transform settings
  QJsonObject transformObject;
  QJsonArray localPositionArray;
  QJsonArray localRotationArray;
  QJsonArray localScaleArray;

  VSTransform* transform = getTransform();
  double* localPos = transform->getLocalPosition();
  double* localRot = transform->getLocalRotation();
  double* localScale = transform->getLocalScale();

  for(int i = 0; i < 3; i++)
  {
    localPositionArray.push_back(localPos[i]);
    localRotationArray.push_back(localRot[i]);
    localScaleArray.push_back(localScale[i]);
  }

  delete[] localPos;
  delete[] localRot;
  delete[] localScale;

  transformObject["LocalPosition"] = localPositionArray;
  transformObject["LocalRotation"] = localRotationArray;
  transformObject["LocalScale"] = localScaleArray;

  json["Transform"] = transformObject;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::readTransformJson(QJsonObject& json)
{
  QJsonObject transformObject = json["Transform"].toObject();
  QJsonArray localPositionArray = transformObject["LocalPosition"].toArray();
  QJsonArray localRotationArray = transformObject["LocalRotation"].toArray();
  QJsonArray localScaleArray = transformObject["LocalScale"].toArray();

  double localPos[3];
  double localRot[3];
  double localScale[3];

  for(int i = 0; i < 3; i++)
  {
    localPos[i] = localPositionArray[i].toDouble();
    localRot[i] = localRotationArray[i].toDouble();
    localScale[i] = localScaleArray[i].toDouble();
  }

  VSTransform* transform = getTransform();
  transform->setLocalPosition(localPos);
  transform->setLocalRotation(localRot);
  transform->setLocalScale(localScale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel* VSAbstractFilter::getModel() const
{
  if(getParentFilter())
  {
    return getParentFilter()->getModel();
  }
  
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSAbstractFilter::getIndex()
{
  VSFilterModel* model = getModel();
  if(model)
  {
    return model->getIndexFromFilter(this);
  }

  return QModelIndex();
}

bool VSAbstractFilter::isEditable() const
{
  return m_Flags && Qt::ItemIsEditable;
}

void VSAbstractFilter::setEditable(bool editable)
{
  if(editable)
  {
    m_Flags |= Qt::ItemIsEditable;
  }
  else
  {
    m_Flags &= ~Qt::ItemIsEditable;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::isCheckable() const
{
  return m_Flags && Qt::ItemIsUserCheckable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setCheckable(bool checkable)
{
  if(checkable)
  {
    m_Flags |= Qt::ItemIsUserCheckable;
  }
  else
  {
    m_Flags &= ~Qt::ItemIsUserCheckable;
    setChecked(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::isChecked()
{
  return isCheckable() && m_Checked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setChecked(bool checked)
{
  m_Checked = checked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState VSAbstractFilter::checkState()
{
  return isChecked() ? Qt::Checked : Qt::Unchecked;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSAbstractFilter::getText() const
{
  if(!m_DisplayText.isEmpty())
  {
    return m_DisplayText;
  }
  return getFilterName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setText(QString display)
{
  m_DisplayText = display;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSAbstractFilter::getToolTip() const
{
  return m_Tooltip;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setToolTip(QString tooltip)
{
  m_Tooltip = tooltip;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFont VSAbstractFilter::font() const
{
  return m_Font;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setFont(QFont font)
{
  m_Font = font;
}
