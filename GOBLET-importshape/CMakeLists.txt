SET(GOBLET_importshape_SOURCES main.cpp insertshape.cpp LineEdge.cpp shapeToGrid.cpp)
SET(GOBLET_importshape_HEADERS insertshape.h LineEdge.h shapeToGrid.h)
QT5_WRAP_CPP(GOBLET_importshape_HEADERS_MOC ${GOBLET_importshape_HEADERS})
QT5_ADD_RESOURCES(GOBLET_importshape_RESOURCES_RCC ${GOBLET_importshape_RESOURCES})

ADD_EXECUTABLE(goblet-importshape ${GOBLET_importshape_SOURCES} ${GOBLET_importshape_HEADERS} ${GOBLET_importshape_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES} ${SHAPELIB_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importshape ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES} ${SHAPELIB_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-importshape)
