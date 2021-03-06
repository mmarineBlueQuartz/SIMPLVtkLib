/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSController.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::VSController(QObject* parent)
: QObject(parent)
, m_FilterModel(new VSFilterModel())
, m_SelectionModel(new QItemSelectionModel(m_FilterModel))
{
  m_ImportObject = new VSConcurrentImport(this);

  connect(m_FilterModel, &VSFilterModel::filterAdded, this, &VSController::listenFilterAdded);
  connect(m_FilterModel, &VSFilterModel::filterRemoved, this, &VSController::filterRemoved);

  // VSConcurrentImport works on another thread, so use the old-style connections to forward signals on the current thread
  connect(m_ImportObject, SIGNAL(blockRender(bool)), this, SIGNAL(blockRender(bool)));
  connect(m_ImportObject, SIGNAL(applyingDataFilters(int)), this, SIGNAL(applyingDataFilters(int)));
  connect(m_ImportObject, SIGNAL(dataFilterApplied(int)), this, SIGNAL(dataFilterApplied(int)));

  connect(m_SelectionModel, &QItemSelectionModel::selectionChanged, this, &VSController::listenSelectionModel);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController::~VSController()
{
  delete m_FilterModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(QString filePath, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(filePath, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadDataContainerArray(VSFileNameFilter* fileFilter, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Reload);
  m_ImportObject->addDataContainerArray(fileFilter, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray(pipeline, dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::reloadPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca)
{
  VSPipelineFilter* parentFilter = dynamic_cast<VSPipelineFilter*>(getFilterModel()->getPipelineFilter(pipeline));
  if(parentFilter)
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::SemiReload);
    m_ImportObject->addDataContainerArray(parentFilter, dca);
  }
  else
  {
    m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Geometry);
    m_ImportObject->addDataContainerArray(pipeline, dca);
  }

  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainerArray(DataContainerArray::Pointer dca)
{
  m_ImportObject->setLoadType(VSConcurrentImport::LoadType::Import);
  m_ImportObject->addDataContainerArray("No File", dca);
  m_ImportObject->run();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importDataContainer(DataContainer::Pointer dc)
{
  SIMPLVtkBridge::WrappedDataContainerPtr wrappedData = SIMPLVtkBridge::WrapDataContainerAsStruct(dc);

  // Add VSSIMPLDataContainerFilter if the DataContainer contains relevant data for rendering
  if(wrappedData)
  {
    VSSIMPLDataContainerFilter* filter = new VSSIMPLDataContainerFilter(wrappedData);
    m_FilterModel->addFilter(filter);
  }

  emit dataImported();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::importData(const QString& filePath)
{
  VSFileNameFilter* textFilter = new VSFileNameFilter(filePath);
  VSDataSetFilter* filter = new VSDataSetFilter(filePath, textFilter);
  // Check if any data was imported
  if(filter->getOutput())
  {
    m_FilterModel->addFilter(textFilter, false);
    m_FilterModel->addFilter(filter);

    emit dataImported();
  }
  else
  {
    textFilter->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilter(VSAbstractFilter* filter)
{
  // Do not change the selection when nullptr is passed in
  if(nullptr == filter || m_FilterModel->getRootFilter() == filter)
  {
    return;
  }

  QModelIndex index = m_FilterModel->getIndexFromFilter(filter);
  m_SelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::saveSession(const QString& sessionFilePath)
{
  QFile outputFile(sessionFilePath);
  if(outputFile.open(QIODevice::WriteOnly))
  {
    QJsonObject rootObj;

    VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
    for(VSAbstractFilter* filter : baseFilters)
    {
      saveFilter(filter, rootObj);
    }

    QJsonDocument doc(rootObj);

    outputFile.write(doc.toJson());
    outputFile.close();

    return true;
  }
  else
  {
    // "Failed to open output file" error
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::saveFilter(VSAbstractFilter* filter, QJsonObject& obj)
{
  QJsonObject rootFilterObj;

  filter->writeJson(rootFilterObj);

  // Write the children
  QJsonObject childrenObj;

  VSAbstractFilter::FilterListType childFilters = filter->getChildren();
  for(VSAbstractFilter* childFilter : childFilters)
  {
    saveFilter(childFilter, childrenObj);
  }

  rootFilterObj["Child Filters"] = childrenObj;

  obj[filter->getFilterName()] = rootFilterObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSController::loadSession(const QString& sessionFilePath)
{
  QFile inputFile(sessionFilePath);
  if(inputFile.open(QIODevice::ReadOnly) == false)
  {
    return false;
  }

  QByteArray byteArray = inputFile.readAll();
  QJsonParseError parseError;

  QJsonDocument doc = QJsonDocument::fromJson(byteArray, &parseError);
  if(parseError.error != QJsonParseError::NoError)
  {
    return false;
  }

  QJsonObject rootObj = doc.object();
  for(QJsonObject::iterator iter = rootObj.begin(); iter != rootObj.end(); iter++)
  {
    QJsonObject filterObj = iter.value().toObject();
    loadFilter(filterObj);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::loadFilter(QJsonObject& obj, VSAbstractFilter* parentFilter)
{
  QUuid uuid(obj["Uuid"].toString());

  VSAbstractFilter* newFilter = nullptr;
  if(uuid == VSClipFilter::GetUuid())
  {
    newFilter = VSClipFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSCropFilter::GetUuid())
  {
    newFilter = VSCropFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSDataSetFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSDataSetFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSFileNameFilter::GetUuid())
  {
    newFilter = VSFileNameFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSMaskFilter::GetUuid())
  {
    newFilter = VSMaskFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSSIMPLDataContainerFilter::GetUuid())
  {
    if(dynamic_cast<VSFileNameFilter*>(parentFilter) != nullptr)
    {
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(parentFilter);
      QString filePath = fileNameFilter->getFilePath();

      newFilter = VSSIMPLDataContainerFilter::Create(filePath, obj, parentFilter);
    }
  }
  else if(uuid == VSSliceFilter::GetUuid())
  {
    newFilter = VSSliceFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSTextFilter::GetUuid())
  {
    newFilter = VSTextFilter::Create(obj, parentFilter);
  }
  else if(uuid == VSThresholdFilter::GetUuid())
  {
    newFilter = VSThresholdFilter::Create(obj, parentFilter);
  }

  if(newFilter != nullptr)
  {
    QJsonObject childrenObj = obj["Child Filters"].toObject();

    m_FilterModel->addFilter(newFilter, (childrenObj.size() == 0));
    newFilter->setChecked(obj["CheckState"].toInt() == Qt::Checked);
    emit filterCheckStateChanged(newFilter);

    for(QJsonObject::iterator iter = childrenObj.begin(); iter != childrenObj.end(); iter++)
    {
      QJsonObject childObj = iter.value().toObject();
      loadFilter(childObj, newFilter);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFileNameFilter* VSController::getBaseFileNameFilter(QString text)
{
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
  for(VSAbstractFilter* baseFilter : baseFilters)
  {
    VSFileNameFilter* filter = dynamic_cast<VSFileNameFilter*>(baseFilter);
    if(filter)
    {
      if(filter->getFilterName().compare(text) == 0)
      {
        return filter;
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getBaseFilters()
{
  return m_FilterModel->getBaseFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getAllFilters()
{
  return m_FilterModel->getAllFilters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel* VSController::getFilterModel()
{
  return m_FilterModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QItemSelectionModel* VSController::getSelectionModel()
{
  return m_SelectionModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSController::getCurrentFilter() const
{
#if 0
  QModelIndex currentIndex = m_SelectionModel->currentIndex();
  return m_FilterModel->getFilterFromIndex(currentIndex);
#else
  VSAbstractFilter::FilterListType selectedFilters = getFilterSelection();
  if(selectedFilters.size() > 0)
  {
    return selectedFilters.front();
  }

  return nullptr;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSController::getFilterSelection() const
{
  VSAbstractFilter::FilterListType filterList;
  QModelIndexList selectionIndices = m_SelectionModel->selectedIndexes();
  for(QModelIndex index : selectionIndices)
  {
    VSAbstractFilter* filter = m_FilterModel->getFilterFromIndex(index);
    filterList.push_back(filter);
  }

  return filterList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::changeFilterSelected(FilterStepChange stepDirection)
{
  switch(stepDirection)
  {
  case FilterStepChange::Parent:
    selectFilterParent();
    break;
  case FilterStepChange::Child:
    selectFilterChild();
    break;
  case FilterStepChange::PrevSibling:
    selectFilterPrevSibling();
    break;
  case FilterStepChange::NextSibling:
    selectFilterNextSibling();
    break;
  default:
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilterParent()
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  if(nullptr == currentFilter)
  {
    // Select the first base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.front());
    }
  }
  else
  {
    // Select the parent filter
    selectFilter(currentFilter->getParentFilter());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilterChild()
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  if(nullptr == currentFilter)
  {
    // Select the last base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.back());
    }
  }
  else
  {
    // Select the first child filter
    selectFilter(currentFilter->getChild(0));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilterPrevSibling()
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  if(nullptr == currentFilter)
  {
    // Select the first base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.front());
    }
  }
  else
  {
    // Select the previous sibling filter
    selectFilter(currentFilter->getPrevSibling());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::selectFilterNextSibling()
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  if(nullptr == currentFilter)
  {
    // Select the last base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.back());
    }
  }
  else
  {
    // Select the next sibling filter
    selectFilter(currentFilter->getNextSibling());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::listenSelectionModel(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);

  if(getCurrentFilter())
  {
    emit filterSelected(getCurrentFilter());
  }
  else
  {
    QModelIndexList selection = m_SelectionModel->selectedIndexes();
    if(selection.size() > 0)
    {
      VSAbstractFilter* filter = m_FilterModel->getFilterFromIndex(selection.first());
      emit filterSelected(filter);
    }
    else
    {
      emit filterSelected(nullptr);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSController::listenFilterAdded(VSAbstractFilter* filter, bool setCurrent)
{
  if(setCurrent)
  {
    selectFilter(filter);
  }
  
  emit filterAdded(filter, setCurrent);
}
