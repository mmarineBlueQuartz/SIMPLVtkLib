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

#pragma once

#include <QtCore/QFutureWatcher>
#include <QtCore/QSemaphore>

#include <QtWidgets/QWidget>

#include <vtkTrivialProducer.h>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerValues.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class SIMPLH5DataReader;

/**
 * @class VSSIMPLDataContainerFilter VSSIMPLDataContainerFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h
 * @brief This class stores a WrappedDataContainerPtr and provides an output port
 * for other filters to connect to for converting SIMPLib DataContainers to something
 * VTK can render.
 */
class SIMPLVtkLib_EXPORT VSSIMPLDataContainerFilter : public VSAbstractDataFilter
{
  Q_OBJECT

public:
  /**
   * @brief Constuctor
   * @param wrappedDataContainer
   * @param parent
   */
  VSSIMPLDataContainerFilter(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDataContainer, VSAbstractFilter* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSSIMPLDataContainerFilter();

  /**
   * @brief Returns true if data set is a 2D image.  Returns false otherwise.
   * @return
   */
  bool isFlatImage() override;

  /**
   * @brief Returns the bounds of the vtkDataSet
   * @return
   */
  double* getBounds() const override;

  /**
   * @brief Returns the output port for the filter
   * @return
   */
  virtual vtkAlgorithmOutput* getOutputPort() override;

  /**
   * @brief Returns the output data for the filter
   */
  VTK_PTR(vtkDataSet) getOutput() const override;

  /**
   * @brief Returns the filter's name
   * @return
   */
  QString getFilterName() const override;

  /**
   * @brief Returns the tooltip to use for the filter
   * @return
   */
  virtual QString getToolTip() const override;

  /**
   * @brief getUuid
   * @return
   */
  static QUuid GetUuid();

  /**
   * @brief Attempts to finish wrapping the DataContainer. Returns true if it succeeded.
   * This method is not thread safe.
   * @return
   */
  bool finishWrapping();

  /**
   * @brief Returns the WrappedDataContainerPtr used by the filter
   * @return
   */
  SIMPLVtkBridge::WrappedDataContainerPtr getWrappedDataContainer() const;

  /**
   * @brief Sets the WrappedDataContainerPtr used by the filter
   * @param wrappedDc The WrappedDataContainerPtr
   */
  void setWrappedDataContainer(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc);

  /**
   * @brief Creates a SIMPLDataContainer filter from the source .dream3d file and json object
   * @param json
   */
  static VSSIMPLDataContainerFilter* Create(const QString& filePath, QJsonObject& json, VSAbstractFilter* parent);

  /**
   * @brief Writes values to a json file from the filter
   * @param json
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @brief reloadData
   */
  void reloadData() override;

  /**
   * @brief reloadData
   * @param dc
   */
  void reloadData(DataContainer::Pointer dc);

  /**
   * @brief Returns true if the data has been fully wrapped and loaded into a vtkDataSet. Returns false otherwise.
   * @return
   */
  bool dataFullyLoaded();

  /**
   * @brief Returns the filter values associated with the filter
   * @return
   */
  VSAbstractFilterValues* getValues() override;

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filter.  Returns false otherwise.
   * @param
   * @return
   */
  static bool CompatibleWithParent(VSAbstractFilter* filter);

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the filter.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override;

public slots:
  /**
   * @brief Wrap the entire DataContainer
   * This method is not thread safe.
   */
  void apply();

  /**
   * @brief This slot is called when a data container is finished being wrapped on a separate thread
   */
  void reloadWrappingFinished();

signals:
  void finishedWrapping();

protected:
  /**
   * @brief Initializes the trivial producer and connects it to the vtkMapper
   */
  void createFilter() override;

private:
  VTK_PTR(vtkTrivialProducer) m_TrivialProducer = nullptr;
  QFutureWatcher<void> m_WrappingWatcher;
  QSemaphore m_ApplyLock;
  bool m_WrappingTransform = false;
  VSSIMPLDataContainerValues* m_DCValues = nullptr;
};
