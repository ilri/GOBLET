SET(GOBLET_genaggregatestats_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_genaggregatestats_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_genaggregatestats_HEADERS_MOC ${GOBLET_genaggregatestats_HEADERS})

QT4_ADD_RESOURCES(GOBLET_genaggregatestats_RESOURCES_RCC ${GOBLET_genaggregatestats_RESOURCES})

ADD_EXECUTABLE(goblet-genaggregatestats ${GOBLET_genaggregatestats_SOURCES} ${GOBLET_genaggregatestats_HEADERS} ${GOBLET_genaggregatestats_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-genaggregatestats ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-genaggregatestats ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-genaggregatestats)
