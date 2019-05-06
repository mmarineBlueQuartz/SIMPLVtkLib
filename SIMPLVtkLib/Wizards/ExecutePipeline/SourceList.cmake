set(${PROJECT_NAME}_ExecutePipeline_HDRS
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/AdvancedPipelineExecutionPage.h
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/ExecutePipelineConstants.h
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/ExecutePipelineWizard.h
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/PipelineExecutionTypeSelectionPage.h
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/PipelineWorker.h
)

set(${PROJECT_NAME}_ExecutePipeline_SRCS
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/AdvancedPipelineExecutionPage.cpp
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/ExecutePipelineWizard.cpp
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/PipelineExecutionTypeSelectionPage.cpp
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/PipelineWorker.cpp
)

set(${PROJECT_NAME}_ExecutePipeline_UIS
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/UI_Files/AdvancedPipelineExecutionPage.ui
  ${WIZARDS_SOURCE_DIR}/ExecutePipeline/UI_Files/PipelineExecutionTypeSelectionPage.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_ExecutePipeline_HDRS}" "${${PROJECT_NAME}_ExecutePipeline_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/ExecutePipeline/UI_Files" "${${PROJECT_NAME}_ExecutePipeline_UIS}" "")
# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_ExecutePipeline_Generated_UI_HDRS ${${PROJECT_NAME}_ExecutePipeline_UIS} )
foreach(h ${${PROJECT_NAME}_ExecutePipeline_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_ExecutePipeline_Generated_UI_HDRS}" "" "0")

include_directories(${WIZARDS_SOURCE_DIR}/ExecutePipeline/UI_Files)