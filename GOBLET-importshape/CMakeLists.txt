SET(GOBLET_importshape_SOURCES main.cpp insertshape.cpp LineEdge.cpp shapeToGrid.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_importshape_HEADERS insertshape.h LineEdge.h shapeToGrid.h ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_importshape_HEADERS_MOC ${GOBLET_importshape_HEADERS})

QT4_ADD_RESOURCES(GOBLET_importshape_RESOURCES_RCC ${GOBLET_importshape_RESOURCES})

ADD_EXECUTABLE(goblet-importshape ${GOBLET_importshape_SOURCES} ${GOBLET_importshape_HEADERS} ${GOBLET_importshape_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${SHAPELIB_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${SHAPELIB_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-importshape)
