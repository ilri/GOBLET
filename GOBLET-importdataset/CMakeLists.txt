SET(GOBLET_importdataset_SOURCES main.cpp gridtocsv.cpp )
SET(GOBLET_importdataset_HEADERS gridtocsv.h)
QT5_WRAP_CPP(GOBLET_importdataset_HEADERS_MOC ${GOBLET_importdataset_HEADERS})
QT5_ADD_RESOURCES(GOBLET_importdataset_RESOURCES_RCC ${GOBLET_importdataset_RESOURCES})

ADD_EXECUTABLE(goblet-importdataset ${GOBLET_importdataset_SOURCES} ${GOBLET_importdataset_HEADERS} ${GOBLET_importdataset_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importdataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importdataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-importdataset)
