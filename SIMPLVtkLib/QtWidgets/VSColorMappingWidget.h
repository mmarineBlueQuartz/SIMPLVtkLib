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

#pragma once

#include <QtWidgets/QWidget>

#include "SVWidgetsLib/Dialogs/ColorPresetsDialog.h"

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "ui_VSColorMappingWidget.h"

class SIMPLVtkLib_EXPORT VSColorMappingWidget : public QWidget
{
  Q_OBJECT

public:
  VSColorMappingWidget(QWidget* parent = nullptr);
  virtual ~VSColorMappingWidget() = default;

public slots:
  /**
   * @brief Changes the filter being displayed
   * @param filter
   * @param filterWidget
   */
  void setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget);

  /**
   * @brief Changes the VSViewController used for fetching VSFilterViewSettings
   * @param viewController
   */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

protected slots:
  /**
   * @brief Slot for handling user interaction with the mapScalarsComboBox
   * @param colorMappingIndex
   */
  void setScalarsMapped(int colorMappingIndex);

  /**
   * @brief Slot for handling user interaction with the showScalarBarCheckBox
   * @param checkState
   */
  void setScalarBarVisible(int checkState);

  /**
   * @brief Slot for creating color preset dialog
   */
  void selectPresetColors();

  /**
   * @brief Slot for handling color preset data
   * @param preset
   * @param pixmap
   */
  void loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap);

  /**
   * @brief Slot for inverting the current scalar bar
   */
  void invertScalarBar();

  /**
   * @brief Slot for handling user interaction with the alphaSlider for object transparency
   * @param value
   */
  void alphaSliderMoved(int value);

  /**
   * @brief Listens for the active VSFilterViewSettings map colors value to change
   * @param settings
   * @param colorMapping
   */
  void listenMapColors(VSFilterViewSettings::ColorMapping colorMapping);

  /**
   * @brief Listens for the active VSFilterViewSettings alpha value to change
   * @param alpha
   */
  void listenAlpha(double alpha);

  /**
   * @brief Listens for the active VSFilterViewSettings scalar bar visibility to change
   * @param show
   */
  void listenScalarBar(bool show);

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();

  /**
   * @brief Updates the information on the VSFilterViewSettings
   */
  void updateViewSettingInfo();

  /**
   * @brief Connects to the given VSFilterViewSettings to take advantage of its signals and slots
   * @param settings
   */
  void connectFilterViewSettings(VSFilterViewSettings* settings);

private:
  QSharedPointer<Ui::VSColorMappingWidget> m_Ui;
  VSAbstractFilter* m_Filter = nullptr;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  VSFilterViewSettings* m_ViewSettings = nullptr;
  ColorPresetsDialog* m_presetsDialog = nullptr;
};
