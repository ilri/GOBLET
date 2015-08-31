SET(GOBLET_calcinshape_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_calcinshape_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_calcinshape_HEADERS_MOC ${GOBLET_calcinshape_HEADERS})

QT4_ADD_RESOURCES(GOBLET_calcinshape_RESOURCES_RCC ${GOBLET_calcinshape_RESOURCES})

ADD_EXECUTABLE(goblet-calcinshape ${GOBLET_calcinshape_SOURCES} ${GOBLET_calcinshape_HEADERS} ${GOBLET_calcinshape_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-calcinshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-calcinshape ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-calcinshape)
