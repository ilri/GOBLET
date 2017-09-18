SET(GOBLET_aggregatetoshape_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_aggregatetoshape_HEADERS_MOC )
QT5_ADD_RESOURCES(GOBLET_aggregatetoshape_RESOURCES_RCC ${GOBLET_aggregatetoshape_RESOURCES})

ADD_EXECUTABLE(goblet-aggregatetoshape ${GOBLET_aggregatetoshape_SOURCES} ${GOBLET_aggregatetoshape_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatetoshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatetoshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-aggregatetoshape)
