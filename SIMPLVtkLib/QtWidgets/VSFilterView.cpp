/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSFilterView.h"

#include <QtWidgets/QMenu>

#include "SIMPLVtkLib/QtWidgets/VSFilterViewDelegate.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterView::VSFilterView(QWidget* parent)
: QTreeView(parent)
{
  connectSlots();
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterView::setupGui()
{
  setHeaderHidden(true);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setContextMenuPolicy(Qt::CustomContextMenu);

  setItemDelegate(new VSFilterViewDelegate(this));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterView::insertFilter(VSAbstractFilter* filter, bool currentFilter)
{
  Q_UNUSED(currentFilter)

  VSFilterViewModel* filterModel = dynamic_cast<VSFilterViewModel*>(model());
  QModelIndex index = filterModel->getIndexFromFilter(filter);
  expand(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterView::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  // Disconnect from the old view controller
  if(m_ViewWidget)
  {
    disconnect(m_ViewWidget->getController(), &VSController::filterAdded, this, &VSFilterView::insertFilter);
  }

  m_ViewWidget = viewWidget;

  if(nullptr == m_ViewWidget)
  {
    setModel(nullptr);
    return;
  }

  setModel(viewWidget->getFilterViewModel());
  setRootIndex(viewWidget->getFilterViewModel()->rootIndex());
  setSelectionModel(viewWidget->getSelectionModel());
  expandAll();

  // Connect to the new view controller
  connect(m_ViewWidget->getController(), &VSController::filterAdded, this, &VSFilterView::insertFilter);
  connect(m_ViewWidget->getFilterViewModel(), &VSFilterViewModel::rootChanged, [=] { setRootIndex(m_ViewWidget->getFilterViewModel()->rootIndex()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterView::connectSlots()
{
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(requestContextMenu(const QPoint&)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterView::requestContextMenu(const QPoint& pos)
{
  activateWindow();

  QModelIndex index = indexAt(pos);

  QPoint mapped;
  if(index.isValid())
  {
    // Note: We must map the point to global from the viewport to
    // account for the header.
    mapped = viewport()->mapToGlobal(pos);
  }
  else
  {
    index = QModelIndex();
    mapped = mapToGlobal(pos);
  }

  QMenu menu;
  if(index.isValid())
  {
    VSAbstractFilter* filter = getFilterFromIndex(index);
    if(filter != nullptr)
    {
      VSAbstractDataFilter* dataFilter = dynamic_cast<VSAbstractDataFilter*>(filter);
      VSFileNameFilter* fileNameFilter = dynamic_cast<VSFileNameFilter*>(filter);
      if(dataFilter != nullptr)
      {
        QAction* reloadAction = new QAction("Reload Data");
        connect(reloadAction, &QAction::triggered, [=] { emit reloadFilterRequested(dataFilter); });
        menu.addAction(reloadAction);

        VSSIMPLDataContainerFilter* dcFilter = dynamic_cast<VSSIMPLDataContainerFilter*>(filter);
        if(dcFilter != nullptr)
        {
          QAction* renameAction = new QAction("Rename");
          connect(renameAction, &QAction::triggered, [=] { emit renameFilterRequested(dataFilter); });
          menu.addAction(renameAction);

          QAction* saveAction = new QAction("Save");
          connect(saveAction, &QAction::triggered, [=] { emit saveFilterRequested(dataFilter); });
          menu.addAction(saveAction);
        }
        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }
      }
      else if(fileNameFilter != nullptr)
      {
        QAction* reloadAction = new QAction("Reload File");
        connect(reloadAction, &QAction::triggered, [=] { emit reloadFileFilterRequested(fileNameFilter); });
        menu.addAction(reloadAction);

        {
          QAction* separator = new QAction(this);
          separator->setSeparator(true);
          menu.addAction(separator);
        }
      }

      QAction* deleteAction = new QAction("Delete");
      connect(deleteAction, &QAction::triggered, [=] {
        VSAbstractFilter* filter = getFilterFromIndex(index);
        emit deleteFilterRequested(filter);
      });
      menu.addAction(deleteAction);
    }

    menu.exec(mapped);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterView::getFilterFromIndex(const QModelIndex& index)
{
  VSFilterViewModel* filterModel = dynamic_cast<VSFilterViewModel*>(model());
  return filterModel->getFilterFromIndex(index);
}
