SET(GOBLET_classifydataset_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_classifydataset_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_classifydataset_RESOURCES_RCC ${GOBLET_classifydataset_RESOURCES})

ADD_EXECUTABLE(goblet-classifydataset ${GOBLET_classifydataset_SOURCES} ${GOBLET_classifydataset_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifydataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifydataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-classifydataset)
