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

#include <QtWidgets/QWidget>

#include "VSAbstractFilter.h"

#include <vtkBox.h>
#include <vtkExtractVOI.h>
#include <vtkImageData.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropValues.h"

/**
 * @class VSCropFilter VSCropFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h
 * @brief This class is a visibility filter that crops a vtkDataSet to X, Y,
 * and Z bounds. This class can be chained with other VSAbstractFilters to
 * further specify the data allowed to be visualized. This filter requires
 * the incoming data type to be a vtkImageData, thus restricting it to following
 * VSSIMPLDataContainerFilters.
 */
class SIMPLVtkLib_EXPORT VSCropFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSCropFilter(VSAbstractFilter* parent = nullptr);

  /**
   * @brief Copy constructor
   * @param copy
   */
  VSCropFilter(const VSCropFilter& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSCropFilter() = default;

  /**
   * @brief Create
   * @param json
   * @return
   */
  static VSCropFilter* Create(QJsonObject& json, VSAbstractFilter* parent);

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
   * @brief Convenience method for determining what the filter does
   * @return
   */
  FilterType getFilterType() const override;

  /**
   * @brief Applies the given values to the filter
   * @param values
   */
  void applyValues(VSCropValues* values);

  /**
   * @brief Applies the crop filter with the given volume of interest and sample rate
   * @param voi
   * @param sampleRate
   */
  Q_INVOKABLE void apply(int voi[6], int sampleRate[3]);

  /**
   * @brief Applies the crop filter with the given volume of interst and sample rate
   * @param voi
   * @param sampleRate
   */
  Q_INVOKABLE void apply(std::vector<int> voiXMin, std::vector<int> sampleRate);

  /**
   * @brief Returns the output port to be used by vtkMappers and subsequent filters
   * @return
   */
  virtual vtkAlgorithmOutput* getOutputPort() override;

  /**
   * @brief Returns a smart pointer containing the output data from the filter
   * @return
   */
  virtual VTK_PTR(vtkDataSet) getOutput() const override;

  /**
   * @brief Returns the output data type
   * @return
   */
  dataType_t getOutputType() const override;

  /**
   * @brief Returns the required incoming data type
   * @return
   */
  static dataType_t GetRequiredInputType();

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filter.  Returns false otherwise.
   * @param filter
   * @return
   */
  static bool CompatibleWithParent(VSAbstractFilter* filter);

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filters.  Returns false otherwise.
   * @param filters
   * @return
   */
  static bool CompatibleWithParents(VSAbstractFilter::FilterListType filters);

  /**
   * @brief Returns the filter values associated with the filter
   * @return
   */
  VSAbstractFilterValues* getValues() override;

  /**
   * @brief Writes values to a json file from the filter
   * @param json
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @brief getUuid
   * @return
   */
  static QUuid GetUuid();

protected:
  /**
   * @brief Initializes the algorithm and connects it to the vtkMapper
   */
  void createFilter() override;

  /**
   * @brief This method updates the input port and connects it to the vtkAlgorithm if it exists
   * @param filter
   */
  void updateAlgorithmInput(VSAbstractFilter* filter) override;

private:
  VTK_PTR(vtkExtractVOI) m_CropAlgorithm;
  VSCropValues* m_CropValues = nullptr;
};

Q_DECLARE_METATYPE(VSCropFilter)
