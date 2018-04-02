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

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <QtCore/QObject>

#include <vtkAxesTransformWidget.h>
#include <vtkAffineWidget.h>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
* @class VSMoveWidget VSMoveWidget.h SIMPLVtkLib/Visualization/VtkWidgets/VSMoveWidget.h
* @brief This class is used to modify a visualization filter's transformation through the 3D interface.
*/
class SIMPLVtkLib_EXPORT VSMoveWidget : public QObject
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  * @param viewWidget
  */
  VSMoveWidget(QObject* parent, VSAbstractViewWidget* viewWidget);

  /**
  * @brief Deconstructor
  */
  virtual ~VSMoveWidget() = default;

  /**
  * @brief Set the view widget used to render
  * @param viewWidget
  */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

  /**
  * @brief Sets the filter to translate
  * @param filter
  */
  void setFilter(VSAbstractFilter* filter);

  /**
  * @brief Enables the vtkWidget for rendering
  */
  void enable();

  /**
  * @brief Disables the vtkWidget for rendering
  */
  void disable();

  /**
  * @brief Returns true if the widget is enabled.  Returns false otherwise.
  * @return
  */
  bool isEnabled();

  /**
  * @brief Sets the enabled state based on the boolean value provided.
  * @param enabled
  */
  void setEnabled(bool enabled);

protected:
  /**
  * @brief Returns true if the object should be rendered and false otherwise
  */
  bool shouldRender();

private:
  //VTK_PTR(vtkAxesTransformWidget) m_TransformWidget;
  VTK_PTR(vtkAffineWidget) m_TransformWidget;
  VSAbstractViewWidget* m_ViewWidget;
  VSAbstractFilter* m_Filter;

  bool m_Enabled;
};