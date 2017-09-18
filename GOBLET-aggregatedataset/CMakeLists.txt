SET(GOBLET_aggregatedataset_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_aggregatedataset_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_aggregatedataset_RESOURCES_RCC ${GOBLET_aggregatedataset_RESOURCES})

ADD_EXECUTABLE(goblet-aggregatedataset ${GOBLET_aggregatedataset_SOURCES} ${GOBLET_aggregatedataset_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatedataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatedataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-aggregatedataset)
