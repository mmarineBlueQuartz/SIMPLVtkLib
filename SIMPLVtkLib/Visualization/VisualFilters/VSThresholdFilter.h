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

#ifndef _VSThresholdFilter_h_
#define _VSThresholdFilter_h_

#include <QtWidgets/QWidget>

#include "Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkThreshold;
class vtkImageData;
class vtkDataSet;
class vtkUnstructuredGrid;
class vtkImplicitDataSet;
class vtkExtractGeometry;
class vtkExtractUnstructuredGrid;
class VSThresholdWidget;
class VSDataSetFilter;
class vtkExtractSelectedThresholds;
class vtkSelection;
class vtkSelectionNode;
class vtkTrivialProducer;
class vtkPointData;
class vtkCelldata;
class vtkMergeFilter;

/**
 * @class VSThresholdFilter VSThresholdFilter.h 
 * SIMPLView/VtkSIMPL/VisualFilters/VSThresholdFilter.h
 * @brief This class handles the thresholding over a given range for a 
 * specified array. This array does not have to match the data array being 
 * visualized and is set separately within the filter. As this class inherits
 * from VSAbstractFilter, it can be chained with other filters to further
 * specify what part of the volume should be visualized.
 */
class SIMPLVtkLib_EXPORT VSThresholdFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
  * @brief Consructor
  * @param parentWidget
  * @param parent
  */
  VSThresholdFilter(VSAbstractFilter* parent);

  /**
  * @brief Deconstructor
  */
  ~VSThresholdFilter();

  /**
  * @brief Sets the filter bounds
  * @param bounds
  */
  void setBounds(double* bounds) override;

  /**
  * @brief Initializes the algorithm and connects it to the vtkMapper
  */
  void setFilter() override;

  /**
  * @brief Returns the filter name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the VSAbstractWidget used by the filter
  */
  //VSAbstractWidget* getWidget() override;

  /**
  * @brief Applies updated values to the filter and calculates the output
  */
  void apply() override;

  /**
  * @brief Returns the output data type
  * @return
  */
  dataType_t getOutputType() override;

  /**
  * @brief Returns the required input data type
  * @return
  */
  static dataType_t getRequiredInputType();

public slots:
  /**
  * @brief Sets the array id to threshold over
  * @param id
  */
  void setThresholdScalarId(int id);

private:
  int m_CurrentId;
  int m_LastId;

  VTK_PTR(vtkThreshold) m_ThresholdAlgorithm;

  //VSThresholdWidget* m_ThresholdWidget;
};

#endif /* _VSThresholdFilter_h_ */