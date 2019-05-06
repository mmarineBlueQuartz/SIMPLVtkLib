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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

/**
 * @class VSAbstractDataFilter VSAbstractDataFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h
 * @brief This is the base class for classes that import data into the project.
 * Its primary purpose is to have a common ancestor and handle required methods
 * from VSAbstractFilter that are not used or have differing applications between
 * data filters.
 */
class SIMPLVtkLib_EXPORT VSAbstractDataFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
   * @brief Deconstructor
   */
  virtual ~VSAbstractDataFilter() = default;

  /**
   * @brief Returns the required input data type
   * @return
   */
  static dataType_t GetRequiredInputType();

  /**
   * @brief Returns the output data type for the filter
   * @return
   */
  dataType_t getOutputType() const override;

  /**
   * @brief Reloads the data in the item
   */
  virtual void reloadData();

  /**
   * @brief Returns true if the data has been imported.  Returns false otherwise.
   * @return
   */
  bool isDataImported() const override;

  /**
   * @brief Convenience method for determining what the filter does
   * @return
   */
  FilterType getFilterType() const override;

  /**
   * @brief Returns true if data set is a 2D image.  Returns false otherwise.
   * @return
   */
  virtual bool isFlatImage() = 0;

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the filter.
   */
  virtual QString getInfoString(SIMPL::InfoStringFormat format) const = 0;

signals:
  void dataReloaded();
  void filterReloaded(VSAbstractFilter* filter);

protected:
  /**
   * @brief Constructor
   */
  VSAbstractDataFilter();

  /**
   * @brief This method is empty as there should never be a case where a VSAbstractDataFilter
   * takes input from another filter.
   * @param filter
   */
  void updateAlgorithmInput(VSAbstractFilter* filter) override;

private:
  bool m_DataImported = false;
};
