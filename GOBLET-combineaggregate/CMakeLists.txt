SET(GOBLET_combineaggregate_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_combineaggregate_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_combineaggregate_RESOURCES_RCC ${GOBLET_combineaggregate_RESOURCES})

ADD_EXECUTABLE(goblet-combineaggregate ${GOBLET_combineaggregate_SOURCES} ${GOBLET_combineaggregate_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combineaggregate ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combineaggregate ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-combineaggregate)
