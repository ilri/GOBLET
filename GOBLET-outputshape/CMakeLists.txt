SET(GOBLET_outputshape_SOURCES main.cpp writeshapefile.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_outputshape_HEADERS writeshapefile.h ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_outputshape_HEADERS_MOC ${GOBLET_outputshape_HEADERS})

QT4_ADD_RESOURCES(GOBLET_outputshape_RESOURCES_RCC ${GOBLET_outputshape_RESOURCES})

ADD_EXECUTABLE(goblet-outputshape ${GOBLET_outputshape_SOURCES} ${GOBLET_outputshape_HEADERS} ${GOBLET_outputshape_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${SHAPELIB_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${SHAPELIB_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-outputshape)
