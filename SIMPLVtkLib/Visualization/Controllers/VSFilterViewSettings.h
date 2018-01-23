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

#ifndef _vsfilterviewsettings_h_
#define _vsfilterviewsettings_h_

#include <QtCore/QObject>

#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

/**
* @class VSFilterViewSettings VSFilterViewSettings.h
* SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h
* @brief This class handles the visibility settings regarding a given VSVisualFilter
* for a single VSViewController. This includes setting the vtkDataArray and component 
* to color by, whether or not to map values to a lookup table, and the visibility of
* both the vtkActor and vtkScalarBarWidget.
*/
class VSFilterViewSettings : public QObject
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param filter
  */
  VSFilterViewSettings(VSAbstractFilter* filter);

  /**
  * @brief Copy constructor
  * @param copy
  */
  VSFilterViewSettings(const VSFilterViewSettings& copy);

  /**
  * @brief Returns a pointer to the VSAbstractFilter
  * @return
  */
  VSAbstractFilter* getFilter();

  /**
  * @brief Returns true if the filter is displayed in for this view.  Returns false otherwise
  * @return
  */
  bool getVisible();

  /**
  * @brief Returns the active array index used to render the filter
  * @return
  */
  int getActiveArrayIndex();

  /**
  * @brief Returns the active array component used to render the filter
  * @return
  */
  int getActiveComponentIndex();

  /**
  * @brief Returns true if the filter values are mapped to a lookup table.  Returns false otherwise
  * @return
  */
  bool getMapColors();

  /**
  * @brief Returns true if the ScalarBarWidget is visible.  Returns false otherwise
  * @return
  */
  bool isScalarBarVisible();

  /**
  * @brief Returns the vtkActor used to render the filter
  * @return
  */
  VTK_PTR(vtkActor) getActor();

  /**
  * @brief Returns the vtkScalarBarWidget used for the filter
  * @return
  */
  VTK_PTR(vtkScalarBarWidget) getScalarBarWidget();

public slots:
  /**
  * @brief Displays the vtkActor for this view
  */
  void show();

  /**
  * @brief Hides the vtkActor for this view
  */
  void hide();

  /**
  * @brief Sets the vtkActor's visibility for this view
  * @param visible
  */
  void setVisible(bool visible);

  /**
  * @brief Updates the active array index for this view
  * @param index
  */
  void setActiveArrayIndex(int index);

  /**
  * @brief Updates the active component index for this view
  * @param index
  */
  void setActiveComponentIndex(int index);

  /**
  * @brief Updates whether or not the data values are mapped to the lookup table for this view
  * @param mapColors
  */
  void setMapColors(bool mapColors);

  /**
  * @brief Updates whether or not the vtkScalarBarWidget is visible for this view
  * @param visible
  */
  void setScalarBarVisible(bool visible);

signals:
  void visibilityChanged(bool);
  void activeArrayIndexChanged(int);
  void activeComponentIndexChanged(int);
  void mapColorsChanged(bool);
  void showScalarBarChanged(bool);

protected:
  /**
  * @brief Performs initial setup commands for any actors used in the view settings
  */
  void setupActors();

private:
  VSAbstractFilter* m_Filter = nullptr;
  bool m_ShowFilter = true;
  int m_ActiveArray = 0;
  int m_ActiveComponent = 0;
  bool m_MapColors = true;
  VTK_PTR(vtkMapper) m_Mapper = nullptr;
  VTK_PTR(vtkActor) m_Actor = nullptr;
  bool m_ShowScalarBar = true;
  VSLookupTableController* m_LookupTable = nullptr;
  VTK_PTR(vtkScalarBarActor) m_ScalarBarActor = nullptr;
  VTK_PTR(vtkScalarBarWidget) m_ScalarBarWidget = nullptr;
};

#endif