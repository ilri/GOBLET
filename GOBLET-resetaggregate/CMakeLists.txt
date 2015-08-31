SET(GOBLET_resetaggregate_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_resetaggregate_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_resetaggregate_HEADERS_MOC ${GOBLET_resetaggregate_HEADERS})

QT4_ADD_RESOURCES(GOBLET_resetaggregate_RESOURCES_RCC ${GOBLET_resetaggregate_RESOURCES})

ADD_EXECUTABLE(goblet-resetaggregate ${GOBLET_resetaggregate_SOURCES} ${GOBLET_resetaggregate_HEADERS} ${GOBLET_resetaggregate_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-resetaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-resetaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-resetaggregate)
