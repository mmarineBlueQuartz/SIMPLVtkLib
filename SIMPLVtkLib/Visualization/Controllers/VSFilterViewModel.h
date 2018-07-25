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

#include <QtCore/QAbstractItemModel>

#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"

/**
 * @class VSFilterViewModel VSFilterViewModel.h SIMPLVtkLib/QtWidgets/VSFilterViewModel.h
 * @brief This class handles the visual filter model for the VSController alongside a
 * VSFilterViewSettings map for the filters.
 */
class SIMPLVtkLib_EXPORT VSFilterViewModel : public QAbstractItemModel
{
  Q_OBJECT

  Q_PROPERTY(QModelIndex rootIndex READ getRootIndex NOTIFY rootChanged)

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSFilterViewModel(QObject* parent = nullptr);

  /**
   * @brief Copy constructor
   * @param model
   */
  VSFilterViewModel(const VSFilterViewModel& model);

  /**
   * @brief Deconstructor
   */
  virtual ~VSFilterViewModel() = default;

  /**
   * @brief Returns the QObject parent since the parent() method is overloaded
   * @return
   */
  QObject* parentObject() const;

  /**
   * @brief Sets the FilterModel to use as a base
   * @param filterModel
   */
  void setFilterModel(VSFilterModel* filterModel);

  /**
   * @brief Sets the VSFilterModel and deep copies the VSFilterViewSettings
   * @param model
   */
  void deepCopy(const VSFilterViewModel& model);

  /**
   * @brief Returns the Qt::ItemFlags for the given index
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the number of columns under the given parent index
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the data for the given index and requested role
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

  /**
   * @brief Sets the data for the given index and role.  Returns true if the process was successful.  Returns false otherwise.
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

  /**
   * @brief Returns the QModelIndex for the row and column under the specified parent index
   * @param row
   * @param column
   * @param parent
   * @return
   */
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
  * @brief Returns the visual filter stored at the given index
  * @param index
  * @return
  */
  VSAbstractFilter* getFilterFromIndex(const QModelIndex& index) const;

  /**
  * @brief Returns the model index of the given filter
  * @param filter
  * @return
  */
  QModelIndex getIndexFromFilter(VSAbstractFilter* filter);

  /**
   * @brief Returns the QModelIndex for the parent of the given index
   * @param index
   * @return
   */
  QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the number of rows belonging to the given parent
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the root index for the model
   * @return
   */
  Q_INVOKABLE QModelIndex getRootIndex() const;

  /**
   * @brief Returns the filter text at the given index
   * @param index
   * @return
   */
  Q_INVOKABLE QString getFilterText(const QModelIndex& index) const;

  /**
   * @brief Returns the filter font at the given index
   * @param index
   * @return
   */
  Q_INVOKABLE QFont getFilterFont(const QModelIndex& index) const;

  /**
   * @brief Returns whether or not the filter at the given index is checkable
   * @param index
   * @return
   */
  Q_INVOKABLE bool getFilterCheckable(const QModelIndex& index) const;

  /**
   * @brief Returns the Qt::CheckState for the filter at the given index
   * @param index
   * @return
   */
  Q_INVOKABLE Qt::CheckState getFilterCheckState(const QModelIndex& index) const;

  Q_INVOKABLE void setFilterCheckState(const QModelIndex& index, Qt::CheckState checked);

  //////////////////////////////
  // VSFilterViewSettings Map //
  //////////////////////////////
  /**
   * @brief Returns the VSFilterViewSettings for the given filter.
   * @param filter
   * @return
   */
  VSFilterViewSettings* getFilterViewSettings(VSAbstractFilter* filter);

  /**
   * @brief Returns the VSFilterViewSettings for the given model index.
   * @param index
   * @return
   */
  VSFilterViewSettings* getFilterViewSettingsByIndex(const QModelIndex& index);

  /**
   * @brief Returns the container of VSFilterViewSettings
   * @return
   */
  VSFilterViewSettings::Map getFilterViewSettingsMap() const;

  std::vector<VSFilterViewSettings*> getAllFilterViewSettings() const;

signals:
  void viewSettingsCreated(VSFilterViewSettings*);
  void viewSettingsRemoved(VSFilterViewSettings*);
  void rootChanged();

protected:
  /**
   * @brief Creates new VSFilterViewSettings for the given filter.
   * @param filter
   * @return
   */
  VSFilterViewSettings* createFilterViewSettings(VSAbstractFilter* filter);

  /**
   * @brief Clears all VSFilterViewSettings
   */
  void clearFilterViewSettings();

  /**
   * @brief Removes the VSFilterViewSetting for the given filter.
   * @aram filter
   * @return
   */
  void removeFilterViewSettings(VSAbstractFilter* filter);

  /**
  * @brief Returns a QVariant with the Qt::CheckState for the filter at the given index
  * @param index
  * @return
  */
  QVariant getCheckState(const QModelIndex& index) const;

  /**
   * @brief Sets the Qt::CheckState for the filter at the given index
   * @param index
   * @param value
   * @return
   */
  bool setCheckState(const QModelIndex& index, QVariant value);

  void beginInsertingFilter(VSAbstractFilter* filter);
  void beginRemovingFilter(VSAbstractFilter* filter, int row);
  void finishInsertingFilter();
  void finishRemovingFilter();

private:
  VSFilterModel* m_FilterModel = nullptr;
  VSFilterViewSettings::Map m_FilterViewSettings;
};

Q_DECLARE_METATYPE(VSFilterViewModel)
