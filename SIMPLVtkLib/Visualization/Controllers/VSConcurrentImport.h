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

#include <list>
#include <utility>

#include <QtCore/QSemaphore>
#include <QtCore/QFutureWatcher>

#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSController;

/**
* @class VSConcurrentImport VSConcurrentImport.h SIMPLVtkLib/Visualization/Controllers/VSConcurrentImport.h
* @brief This class handles the multithreaded import process for VSSIMPLDataContainerFilters.
*/
class SIMPLVtkLib_EXPORT VSConcurrentImport : public QObject
{
  Q_OBJECT

public:
  using DcaFilePair = std::pair<VSFileNameFilter*, DataContainerArray::Pointer>;

  /**
  * @brief Constructor
  * @param parent
  */
  VSConcurrentImport(VSController* parent);

  /**
  * @brief Deconstructor
  */
  virtual ~VSConcurrentImport() = default;

  /**
  * @brief Add a DataContainerArray with the given file path to the list items to import
  * @param filePath
  * @param dca
  */
  void addDataContainerArray(QString filePath, DataContainerArray::Pointer dca);

  /**
  * @brief Performs the import process on as many threads as are available.
  * This process is performed one file path at a time, meaning that many small files will 
  * be slower to import than a single large file if the amount of data is the same.
  */
  void run();

protected:
  /**
  * @brief Adds the DcaFilePair value to the list of files and DataContainerArrays to wrap
  * @param wrappedFileDc
  */
  void addDataContainerArray(DcaFilePair wrappedFileDc);

  /**
  * @brief Begins importing the given DataContainerArray and file path pair
  * @param filePair
  */
  void importDataContainerArray(DcaFilePair filePair);

  /**
  * @brief Wraps the DataContainers in vtkDataSets and prepares to add the generated filters to the filter model
  * @param fileFilter
  */
  void importDataContainer(VSFileNameFilter* fileFilter);

  /**
  * @brief Given a VSFileNameFilter and wrapped data container, create and add a VSSIMPLDataContainerFilter to the filter model
  * @param fileFilter
  * @param wrappedDc
  */
  void importWrappedDataContainer(VSFileNameFilter* fileFilter, SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc);

private:
  VSController* m_Controller;
  std::list<DcaFilePair> m_WrappedList;

  QList<DataContainerShPtr> m_ImportDataContainerOrder;
  QSemaphore m_ImportDataContainerOrderLock;
  QVector< QSharedPointer<QFutureWatcher<void>> > m_ImportDataContainerWatchers;
  int m_NumOfFinishedImportDataContainerThreads = 0;
};
