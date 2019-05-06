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

#include <vtkDataSet.h>
#include <vtkTrivialProducer.h>

#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSFilterModel;

/**
 * @class VSRootFilter VSRootFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h
 * @brief This class stores a WrappedDataContainerPtr and provides an output port
 * for other filters to connect to for converting SIMPLib DataContainers to something
 * VTK can render.
 */
class SIMPLVtkLib_EXPORT VSRootFilter : public VSTextFilter
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSRootFilter(VSFilterModel* model);

  /**
   * @brief Deconstructor
   */
  virtual ~VSRootFilter() = default;

  /**
   * @brief Returns the VSFilterModel used for this tree of filters
   * @return
   */
  VSFilterModel* getModel() const override;

  /**
   * @brief Returns the filter name
   * @return
   */
  QString getFilterName() const override;

  /**
   * @brief Convenience method for determining what the filter does
   * @return
   */
  FilterType getFilterType() const override;

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filter.  Returns false otherwise.
   * @param
   * @return
   */
  static bool CompatibleWithParent(VSAbstractFilter* filter);

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

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the filter.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override;

private:
  VSFilterModel* m_Model = nullptr;
};
