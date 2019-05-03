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

#pragma once

#include <QtCore/QFutureWatcher>
#include <QtCore/QSemaphore>

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSAdvancedVisibilitySettingsWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSColorMappingWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSFilterSettingsWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSFilterView.h"
#include "SIMPLVtkLib/QtWidgets/VSInfoWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSTransformWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSVisibilitySettingsWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilterValues.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSMainWidgetBase VSMainWidgetBase.h SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h
 * @brief This class is the superclass for VSMainWidget and contains the base methods
 * for accessing and viewing information regarding visual filters.
 */
class SIMPLVtkLib_EXPORT VSMainWidgetBase : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSMainWidgetBase(QWidget* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSMainWidgetBase() = default;

  /**
   * @brief Returns the VSController for the widget
   */
  VSController* getController();

  /**
   * @brief Returns the active view widget
   * @return
   */
  VSAbstractViewWidget* getActiveViewWidget();

  /**
   * @brief Returns a vector of all view widgets contained in the main widget
   * @return
   */
  std::vector<VSAbstractViewWidget*> getAllViewWidgets();

  /**
   * @brief Returns the VSFilterView used by the main widget. Returns nullptr if none is used
   * @return
   */
  VSFilterView* getFilterView();

  /**
   * @brief Sets the VSFilterView to use, apply the controller's filter model
   * and connect to any signals it might have.
   * @param view
   */
  void setFilterView(VSFilterView* view);

  /**
   * @brief Sets the VSInfoWidget to use for displaying information about the active filter
   * @param infoWidget
   */
  void setInfoWidget(VSInfoWidget* infoWidget);

  /**
   * @brief Returns the VSFilterSettingsWidget
   * @return
   */
  VSFilterSettingsWidget* getFilterSettingsWidget() const;

  /**
   * @brief Returns the VSVisibilitySettingsWidget
   * @return
   */
  VSVisibilitySettingsWidget* getVisibilitySettingsWidget() const;

  /**
   * @brief Returns the VSColorMappingWidget
   * @return
   */
  VSColorMappingWidget* getColorMappingWidget() const;

  /**
   * @brief Returns the VSAdvancedVisibilitySettingsWidget
   * @return
   */
  VSAdvancedVisibilitySettingsWidget* getAdvancedVisibilitySettingsWidget() const;

  /**
   * @brief Returns the VSTransformWidget
   * @return
   */
  VSTransformWidget* getTransformWidget() const;

  /**
   * @brief Sets the VSFilterSettingsWidget
   * @param widget
   */
  void setFilterSettingsWidget(VSFilterSettingsWidget* widget);

  /**
   * @brief Sets the VSVisibilitySettingsWidget
   * @param widget
   */
  void setVisibilitySettingsWidget(VSVisibilitySettingsWidget* widget);

  /**
   * @brief Sets the VSColorMappingWidget
   * @param widget
   */
  void setColorMappingWidget(VSColorMappingWidget* widget);

  /**
   * @brief Sets the VSAdvancedVisibilitySettingsWidget
   * @param widget
   */
  void setAdvancedVisibilityWidget(VSAdvancedVisibilitySettingsWidget* widget);

  /**
   * @brief Sets the VSTransformWidget
   * @param widget
   */
  void setTransformWidget(VSTransformWidget* widget);

  /**
   * @brief importDataContainerArray
   * @param dca
   */
  bool importDataContainerArray(const QString& filePath, DataContainerArray::Pointer dca);

  /**
   * @brief importPipelineOutput
   * @param pipeline
   * @param dca
   * @return
   */
  bool importPipelineOutput(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca);

  /**
   * @brief importPipelineOutput
   * @param pipelines
   * @return
   */
  bool importPipelineOutput(std::vector<FilterPipeline::Pointer> pipelines);

  /**
   * @brief Imports or reloads the given DataContainerArray from the FilterPipeline
   * @param pipeline
   * @param dca
   */
  void importFilterPipeline(FilterPipeline::Pointer pipeline, DataContainerArray::Pointer dca);

public slots:
  /**
   * @brief Create a clip filter and set the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createClipFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Create a crop filter and set the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createCropFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Create a slice filter and set the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createSliceFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Create a mask filter and set the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createMaskFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Create a threshold filter and set the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createThresholdFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Create a text filter and ste the given filter as its parent.  If no filter is provided,
   * the current filter is used instead.
   * @param parents
   */
  void createTextFilter(VSAbstractFilter::FilterListType parents = VSAbstractFilter::FilterListType());

  /**
   * @brief Renders the active view widget
   */
  void renderActiveView();

  /**
   * @brief Renders all view widgets contained in the main widget
   */
  void renderAllViews();

  /**
   * @brief Selects the given filter in the active view
   * @param filter
   */
  void selectFilter(VSAbstractFilter* filter);

  /**
   * @brief Selects the given filters in the active view
   * @param filters
   */
  void selectFilters(VSAbstractFilter::FilterListType filters);

  /**
   * @brief launchHDF5SelectionDialog
   * @param proxy
   */
  void launchHDF5SelectionDialog(const QString& filePath);

signals:
  void changedActiveView(VSAbstractViewWidget* viewWidget);
  void proxyFromFilePathGenerated(DataContainerArrayProxy proxy, const QString& filePath);
  void selectedFiltersChanged(VSAbstractFilter::FilterListType filters);
  void importDataQueueStarted();
  void importDataQueueFinished();

protected:
  /**
   * @brief Connect signals and slots for the VSController
   */
  virtual void connectSlots();

  /**
   * @brief Creates shortcuts for changing, applying, resetting, and deleting the current filter
   */
  void setupShortcuts();

  /**
   * @brief Returns the current visual filter
   * @return
   */
  VSAbstractFilter* getCurrentFilter() const;

  /**
   * @brief Returns the filter selection for the active view
   * @return
   */
  VSAbstractFilter::FilterListType getCurrentSelection() const;

  /**
   * @brief Performs handling adding a filter and toggling parent visibility
   * @param filter
   * @param parent
   */
  void finishAddingFilter(VSAbstractFilter* filter, VSAbstractFilter* parent);

  /**
   * @brief Returns a list of filter widgets that map to the given set of filters
   * @param filters
   * @return
   */
  // VSAbstractFilterWidget::ListType getFilterWidgets(VSAbstractFilter::FilterListType filters);

protected slots:
  /**
   * @brief Notifies change in the active VSAbstractViewWidget
   * @param viewWidget
   */
  virtual void setActiveView(VSAbstractViewWidget* viewWidget);

  /**
   * @brief Resets the active view widget to nullptr
   */
  virtual void activeViewClosed();

  /**
   * @brief setCurrentFilter
   * @param filter
   */
  virtual void setCurrentFilter(VSAbstractFilter* filter);

  /**
   * @brief Change the current filter selection by a step in the given direction.
   * @param stepDirection
   * @param addSelection
   */
  virtual void changeFilterSelected(VSAbstractViewWidget::FilterStepChange stepDirection, bool addSelection = false);

  /**
   * @brief Listens for changes to the active view's filter selection
   * @param selection
   */
  virtual void listenSelectionChanged(QItemSelection selection);

  /**
   * @brief Listens for the current filter to change
   * @param filter
   */
  virtual void listenCurrentFilterChanged(VSAbstractFilter* filter);

  /**
   * @brief Applies the current filter
   */
  virtual void applyCurrentFilter();

  /**
   * @brief Resets the current filter to the last applied values
   */
  virtual void resetCurrentFilter();

  /**
   * @brief Deletes the current filter
   */
  virtual void deleteCurrentFilter();

  /**
   * @brief Toggles the current filter's visibility in the active view
   */
  virtual void toggleCurrentFilterVisibility();

  /**
   * @brief Deletes the given filter and its children from the model and
   * removes them from the view widgets
   * @param filter
   */
  virtual void deleteFilter(VSAbstractFilter* filter);

  /**
   * @brief Deletes all filters
   */
  void clearFilters();

  /**
   * @brief reloadFilter
   * @param filter
   */
  virtual void reloadDataFilter(VSAbstractDataFilter* filter);

  /**
   * @brief renameFilter
   * @param filter
   */
  virtual void renameDataFilter(VSAbstractDataFilter* filter);

  /**
   * @brief reloadFileFilter
   * @param filter
   */
  virtual void reloadFileFilter(VSFileNameFilter* filter);

  /**
   * @brief Connects a new VSAbstractViewWidget to the main widget
   */
  virtual void connectViewWidget(VSAbstractViewWidget* viewWidget);

  /**
   * @brief Handles changes in the active filter view's visibility
   * @param viewSettings
   * @param visible
   */
  // virtual void setFilterVisibility(VSFilterViewSettings* viewSettings, bool visible);

  /**
   * @brief Adds a filter to the model and sets it as the selected filter if requested
   * @param filter
   * @param currentFilter
   */
  void filterAdded(VSAbstractFilter* filter, bool currentFilter = true);

  /**
   * @brief Removes filter from the FilterToFilterWidget map
   * @param filter
   */
  void filterRemoved(VSAbstractFilter* filter);

  /**
   * @brief generateError
   * @param title
   * @param msg
   * @param code
   */
  void generateError(const QString& title, const QString& msg, const int& code);

private:
  VSController* m_Controller = nullptr;
  // VSAbstractFilterWidget::ListType m_CurrentFilterWidgets;
  VSAbstractFilter* m_CurrentFilter = nullptr;
  VSAbstractViewWidget* m_ActiveViewWidget = nullptr;
  VSFilterView* m_FilterView = nullptr;
  VSFilterSettingsWidget* m_FilterSettingsWidget = nullptr;
  VSVisibilitySettingsWidget* m_VisibilitySettingsWidget = nullptr;
  VSColorMappingWidget* m_ColorMappingWidget = nullptr;
  VSTransformWidget* m_TransformWidget = nullptr;
  VSAdvancedVisibilitySettingsWidget* m_AdvancedVisibilityWidget = nullptr;

  // QMap<VSAbstractFilter*, VSAbstractFilterWidget*> m_FilterToFilterWidgetMap;

  /**
   * @brief reloadFilters
   * @param filter
   * @param fileFilter
   */
  void reloadFilters(std::vector<VSAbstractDataFilter*> filter);
};
