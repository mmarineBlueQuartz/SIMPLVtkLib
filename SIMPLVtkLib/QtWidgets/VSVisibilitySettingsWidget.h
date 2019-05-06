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

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "ui_VSVisibilitySettingsWidget.h"

/**
 * @class VSVisibilitySettingsWidget VSVisibilitySettingsWidget.h
 * SIMPLVtkLib/QtWidgets/VSVisibilitySettingsWidget
 * @brief This class handles the visibility settings for VSMainWidgetBase, mainly
 * covering the Representation, DataArray, and Component for a given filter or filters.
 */
class SIMPLVtkLib_EXPORT VSVisibilitySettingsWidget : public QWidget
{
  Q_OBJECT

public:
  VSVisibilitySettingsWidget(QWidget* parent = nullptr);
  virtual ~VSVisibilitySettingsWidget() = default;

public slots:
  /**
   * @brief Changes the filter being displayed
   * @param filters
   * @param filterWidget
   */
  void setFilters(VSAbstractFilter::FilterListType filters);

  /**
   * @brief Changes the VSViewController used for fetching VSFilterViewSettings
   * @param viewController
   */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

protected slots:
  /**
   * @brief Sets the actorRepresentation for the current filter
   * @param index
   */
  void representationComboChanged(int index);

  /**
   * @brief Handles the active array combo box being changed
   * @param index
   */
  void arrayNameComboChanged(const QString& name);

  /**
   * @brief Handles the active component combo box being changed
   * @param index
   */
  void arrayComponentComboChanged(int index);

  /**
   * @brief Slot for handling changes in the VSColorButton used for selecting the VSFilterViewSetting's solid color
   * @param color
   */
  void colorButtonChanged(QColor color);

  /**
   * @brief Listens for the active VSFilterViewSettings representation type to change
   * @param rep
   */
  void listenRepresentationType(VSFilterViewSettings::Representation rep);

  /**
   * @brief Listens for the active VSFilterViewSettings active array name to change
   * @param name
   */
  void listenArrayName(QString name);

  /**
   * @brief Listens for the active VSFilterViewSettings active component index to change
   * @param index
   */
  void listenComponentIndex(int index);

  /**
   * @brief Listens for the active VSFilterViewSettings solid color to change
   */
  void listenSolidColor();
  
  /**
   * @brief Handles the subsampling spin box being changed
   * @param value
   */
  void subsampleValueChanged(int value);

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();
  /**
   * @brief Updates the information on the visual filter
   */
  void updateFilterInfo();

  /**
   * @brief Updates the information on the VSFilterViewSettings
   */
  void updateViewSettingInfo();

  /**
   * @brief Connects to the given VSFilterViewSettings to take advantage of its signals and slots
   * @param settings
   */
  void connectFilterViewSettings(VSFilterViewSettings::Collection settings);

  /**
   * @brief Updates the active array combo box
   * @param arrayName
   */
  void setComboArrayName(QString arrayName);

  /**
   * @brief Updates the active component combo box based on the given array name
   * @param arrayName
   */
  void updateComponentComboBox(QString arrayName);

  /**
   * @brief Returns true if the combo box has an option for multiple values.  Returns false otherwise.
   * @param comboBox
   * @return
   */
  bool hasMultiValueOption(QComboBox* comboBox) const;

  /**
   * @brief Adds a multi-value option to the given QComboBox if it does not have one already
   * @param comboBox
   */
  void addMultiValueOption(QComboBox* comboBox);

  /**
   * @brief Removes the multi-value option from the given QComboBox
   * @param comboBox
   */
  void removeMultiValueOption(QComboBox* comboBox);

private:
  QSharedPointer<Ui::VSVisibilitySettingsWidget> m_Ui;
  VSAbstractFilter::FilterListType m_Filters;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  VSFilterViewSettings::Collection m_ViewSettings;
};