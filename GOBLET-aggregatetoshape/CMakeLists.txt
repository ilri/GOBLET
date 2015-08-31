SET(GOBLET_aggregatetoshape_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_aggregatetoshape_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_aggregatetoshape_HEADERS_MOC ${GOBLET_aggregatetoshape_HEADERS})

QT4_ADD_RESOURCES(GOBLET_aggregatetoshape_RESOURCES_RCC ${GOBLET_aggregatetoshape_RESOURCES})

ADD_EXECUTABLE(goblet-aggregatetoshape ${GOBLET_aggregatetoshape_SOURCES} ${GOBLET_aggregatetoshape_HEADERS} ${GOBLET_aggregatetoshape_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatetoshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatetoshape ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-aggregatetoshape)
