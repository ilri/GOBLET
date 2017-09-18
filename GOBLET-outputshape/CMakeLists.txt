SET(GOBLET_outputshape_SOURCES main.cpp writeshapefile.cpp)
SET(GOBLET_outputshape_HEADERS writeshapefile.h)
QT5_WRAP_CPP(GOBLET_outputshape_HEADERS_MOC ${GOBLET_outputshape_HEADERS})
QT5_ADD_RESOURCES(GOBLET_outputshape_RESOURCES_RCC ${GOBLET_outputshape_RESOURCES})

ADD_EXECUTABLE(goblet-outputshape ${GOBLET_outputshape_SOURCES} ${GOBLET_outputshape_HEADERS} ${GOBLET_outputshape_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES} ${SHAPELIB_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES} ${SHAPELIB_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-outputshape)
