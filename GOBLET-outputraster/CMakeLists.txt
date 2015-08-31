SET(GOBLET_outputraster_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_outputraster_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_outputraster_HEADERS_MOC ${GOBLET_outputraster_HEADERS})

QT4_ADD_RESOURCES(GOBLET_outputraster_RESOURCES_RCC ${GOBLET_outputraster_RESOURCES})

ADD_EXECUTABLE(goblet-outputraster ${GOBLET_outputraster_SOURCES} ${GOBLET_outputraster_HEADERS} ${GOBLET_outputraster_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputraster ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-outputraster ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-outputraster)
