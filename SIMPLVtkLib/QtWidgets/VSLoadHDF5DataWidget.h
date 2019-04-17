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

#include <QtCore/QDateTime>
#include <QtCore/QFutureWatcher>

#include <QtWidgets/QWidget>

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "ui_VSLoadHDF5DataWidget.h"

class DataContainerArrayProxy;

class VSLoadHDF5DataWidget : public QWidget
{
  Q_OBJECT

public:
  VSLoadHDF5DataWidget(QWidget* parent = 0);
  ~VSLoadHDF5DataWidget();

  Q_PROPERTY(DataContainerArrayProxy Proxy READ getProxy WRITE setProxy NOTIFY proxyChanged)

  /**
   * @brief getLoadProxy
   * @return
   */
  DataContainerArrayProxy getProxy();

  /**
   * @brief setProxy
   * @param proxy
   */
  void setProxy(DataContainerArrayProxy proxy);

  /**
   * @brief initialize
   */
  void initialize(const QString& filePath);

  /**
   * @brief setNavigationButtonsVisibility
   * @param visible
   */
  void setNavigationButtonsVisibility(bool visible);

protected slots:
  /**
   * @brief modelDataChanged
   */
  void modelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

  /**
   * @brief selectAllStateChanged
   */
  void selectAllStateChanged(int state);

  /**
   * @brief proxyInitFinished
   */
  void proxyInitFinished();

protected:
  void setupGui();

signals:
  void proxyChanged(DataContainerArrayProxy proxy);
  void loadBtnClicked();
  void cancelBtnClicked();

private:
  QSharedPointer<Ui::VSLoadHDF5DataWidget> m_Ui;

  bool m_LoadingProxy = false;
  QMovie* m_LoadingMovie = nullptr;

  DataContainerArrayProxy m_Proxy;
  QString m_ProxyFilePath;
  QDateTime m_ProxyLastModified;

  QFutureWatcher<DataContainerArrayProxy> m_ProxyInitWatcher;

  /**
   * @brief openDREAM3DFile
   * @param filePath
   * @param instance
   */
  DataContainerArrayProxy readDCAProxy(const QString& filePath);

public:
  VSLoadHDF5DataWidget(const VSLoadHDF5DataWidget&) = delete;            // Copy Constructor Not Implemented
  VSLoadHDF5DataWidget(VSLoadHDF5DataWidget&&) = delete;                 // Move Constructor Not Implemented
  VSLoadHDF5DataWidget& operator=(const VSLoadHDF5DataWidget&) = delete; // Copy Assignment Not Implemented
  VSLoadHDF5DataWidget& operator=(VSLoadHDF5DataWidget&&) = delete;      // Move Assignment Not Implemented
};
