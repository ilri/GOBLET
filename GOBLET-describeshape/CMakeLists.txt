SET(GOBLET_describeshape_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_describeshape_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_describeshape_RESOURCES_RCC ${GOBLET_describeshape_RESOURCES})

ADD_EXECUTABLE(goblet-describeshape ${GOBLET_describeshape_SOURCES} ${GOBLET_describeshape_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-describeshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-describeshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-describeshape)
