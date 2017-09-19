SET(GOBLET_datasetextract_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_datasetextract_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_datasetextract_RESOURCES_RCC ${GOBLET_datasetextract_RESOURCES})

ADD_EXECUTABLE(goblet-datasetextract ${GOBLET_datasetextract_SOURCES} ${GOBLET_datasetextract_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-datasetextract ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
  TARGET_LINK_LIBRARIES(goblet-datasetextract ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-datasetextract)
