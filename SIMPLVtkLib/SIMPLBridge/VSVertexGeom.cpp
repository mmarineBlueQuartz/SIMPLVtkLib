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

#include "VSVertexGeom.h"

#include <cmath>

#include <vtkCellType.h>
#include <vtkCellTypes.h>
#include <vtkIdTypeArray.h>

const int CELL_TYPE = VTK_VERTEX;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVertexGrid* VSVertexGrid::New()
{
  return new VSVertexGrid();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVertexGeom* VSVertexGeom::New()
{
  return new VSVertexGeom();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Elements: " << GetNumberOfCells() << endl;
  os << indent << "CellType: " << vtkCellTypes::GetClassNameFromTypeId(CELL_TYPE) << endl;
  os << indent << "CellSize: " << GetMaxCellSize() << endl;
  os << indent << "NumberOfCells: " << GetNumberOfCells() << endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVertexGeom::VSVertexGeom()
: vtkObject()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::SetGeometry(VertexGeom::Pointer VertexGeom)
{
  m_Geom = VertexGeom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSVertexGeom::GetNumberOfCells()
{
  if(nullptr == m_Geom)
  {
    vtkErrorMacro("Wrapper Geometry missing a Geometry object");
    return -1;
  }

  return m_Geom->getNumberOfElements();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSVertexGeom::GetCellType(vtkIdType cellId)
{
  if(0 == GetNumberOfCells())
  {
    return VTK_EMPTY_CELL;
  }

  return CELL_TYPE;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::GetCellPoints(vtkIdType cellId, vtkIdList* ptIds)
{
  const int numVerts = 1;

  ptIds->SetNumberOfIds(numVerts);
  ptIds->SetId(0, cellId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::GetPointCells(vtkIdType ptId, vtkIdList* cellIds)
{
  // No implementation in VertexGeom
  int count = m_Geom->getVertices()->getNumberOfTuples();

  for(int i = 0; i < count; i++)
  {
    cellIds->SetId(i, i);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSVertexGeom::GetMaxCellSize()
{
  return std::ceil(m_MaxCellSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::GetIdsOfCellsOfType(int type, vtkIdTypeArray* array)
{
  if(CELL_TYPE == type)
  {
    int numValues = GetNumberOfCells();

    array = vtkIdTypeArray::New();
    array->SetNumberOfTuples(numValues);
    array->SetNumberOfComponents(1);

    vtkIdType* arrayValues = new vtkIdType[numValues];
    for(int i = 0; i < numValues; i++)
    {
      arrayValues[i] = i;
    }

    array->SetVoidArray(arrayValues, numValues, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSVertexGeom::IsHomogeneous()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::Allocate(vtkIdType numCells, int extSize)
{
  vtkErrorMacro("Read only container.");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSVertexGeom::InsertNextCell(int type, const vtkIdList ptIds[])
{
  vtkErrorMacro("Read only container.");
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSVertexGeom::InsertNextCell(int type, vtkIdType npts, const vtkIdType ptIds[])
{
  vtkErrorMacro("Read only container.");
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSVertexGeom::InsertNextCell(int type, vtkIdType npts, const vtkIdType ptIds[], vtkIdType nfaces, const vtkIdType faces[])
{
  // VertexGeometry should probably not be modified by VTK calls
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVertexGeom::ReplaceCell(vtkIdType cellId, int npts, const vtkIdType pts[])
{
  vtkErrorMacro("Read only container.");
}
