SET(GOBLET_genaggregatestats_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_genaggregatestats_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_genaggregatestats_RESOURCES_RCC ${GOBLET_genaggregatestats_RESOURCES})

ADD_EXECUTABLE(goblet-genaggregatestats ${GOBLET_genaggregatestats_SOURCES} ${GOBLET_genaggregatestats_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-genaggregatestats ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-genaggregatestats ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-genaggregatestats)
