set(${PROJECT_NAME}_PerformMontage_HDRS
  ${WIZARDS_SOURCE_DIR}/PerformMontage/DatasetListWidget.h
  ${WIZARDS_SOURCE_DIR}/PerformMontage/DisplayTypeSelectionPage.h
  ${WIZARDS_SOURCE_DIR}/PerformMontage/PerformMontageWizard.h
  ${WIZARDS_SOURCE_DIR}/PerformMontage/SaveImageFilePage.h
)

set(${PROJECT_NAME}_PerformMontage_SRCS
  ${WIZARDS_SOURCE_DIR}/PerformMontage/DatasetListWidget.cpp
  ${WIZARDS_SOURCE_DIR}/PerformMontage/DisplayTypeSelectionPage.cpp
  ${WIZARDS_SOURCE_DIR}/PerformMontage/PerformMontageWizard.cpp
  ${WIZARDS_SOURCE_DIR}/PerformMontage/SaveImageFilePage.cpp
)

set(${PROJECT_NAME}_PerformMontage_UIS
  ${WIZARDS_SOURCE_DIR}/PerformMontage/UI_Files/DatasetListWidget.ui
  ${WIZARDS_SOURCE_DIR}/PerformMontage/UI_Files/DisplayTypeSelectionPage.ui
  ${WIZARDS_SOURCE_DIR}/PerformMontage/UI_Files/PipelineExecutionTypeSelectionPage.ui
  ${WIZARDS_SOURCE_DIR}/PerformMontage/UI_Files/SaveImageFilePage.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_PerformMontage_HDRS}" "${${PROJECT_NAME}_PerformMontage_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/PerformMontage/UI_Files" "${${PROJECT_NAME}_PerformMontage_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_PerformMontage_Generated_UI_HDRS ${${PROJECT_NAME}_PerformMontage_UIS} )
foreach(h ${${PROJECT_NAME}_PerformMontage_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_PerformMontage_Generated_UI_HDRS}" "" "0")

include_directories(${WIZARDS_SOURCE_DIR}/PerformMontage/UI_Files)