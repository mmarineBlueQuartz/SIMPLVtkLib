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

#include "VSMoveWidget.h"

#include <vtkAxesTransformRepresentation.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMoveWidget::VSMoveWidget(QObject* parent, VSAbstractViewWidget* viewWidget)
  : QObject(parent)
  , m_Filter(nullptr)
  , m_ViewWidget(nullptr)
  , m_Enabled(false)
{
  m_TransformWidget = VTK_PTR(vtkAffineWidget)::New();
  m_TransformWidget->CreateDefaultRepresentation();

  setViewWidget(viewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMoveWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  if(shouldRender())
  {
    m_TransformWidget->EnabledOff();
  }

  m_ViewWidget = viewWidget;
  
  vtkRenderer* renderer = viewWidget->getVisualizationWidget()->getRenderer();
  m_TransformWidget->SetCurrentRenderer(renderer);
  m_TransformWidget->SetInteractor(viewWidget->getVisualizationWidget()->GetInteractor());
  //m_TransformWidget->GetLineRepresentation()->SetRenderer(renderer);

  if(shouldRender())
  {
    m_TransformWidget->EnabledOn();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMoveWidget::setFilter(VSAbstractFilter* filter)
{
  if(shouldRender())
  {
    m_TransformWidget->EnabledOff();
  }

  bool disabled = (nullptr == m_Filter) && m_Enabled;

  m_Filter = filter;

  if(nullptr == filter)
  {
    //disable();
    return;
  }

  // Set global position
  //m_TransformWidget->GetLineRepresentation()->SetOriginWorldPosition(filter->getTransform()->getPosition());

  if(shouldRender())
  {
    enable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMoveWidget::enable()
{
  bool rendered = shouldRender();

  m_Enabled = true;

  if(!rendered)
  {
    m_TransformWidget->On();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMoveWidget::disable()
{
  bool rendered = shouldRender();

  m_Enabled = false;

  if(rendered)
  {
    m_TransformWidget->Off();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMoveWidget::isEnabled()
{
  return shouldRender();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMoveWidget::setEnabled(bool enabled)
{
  if(enabled)
  {
    enable();
  }
  else
  {
    disable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSMoveWidget::shouldRender()
{
  return m_Enabled && m_ViewWidget && m_Filter;
}
