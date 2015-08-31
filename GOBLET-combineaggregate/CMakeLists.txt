SET(GOBLET_combineaggregate_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_combineaggregate_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_combineaggregate_HEADERS_MOC ${GOBLET_combineaggregate_HEADERS})

QT4_ADD_RESOURCES(GOBLET_combineaggregate_RESOURCES_RCC ${GOBLET_combineaggregate_RESOURCES})

ADD_EXECUTABLE(goblet-combineaggregate ${GOBLET_combineaggregate_SOURCES} ${GOBLET_combineaggregate_HEADERS} ${GOBLET_combineaggregate_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combineaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combineaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-combineaggregate)
