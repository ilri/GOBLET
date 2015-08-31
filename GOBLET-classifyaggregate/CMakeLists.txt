SET(GOBLET_classifyaggregate_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_classifyaggregate_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_classifyaggregate_HEADERS_MOC ${GOBLET_classifyaggregate_HEADERS})

QT4_ADD_RESOURCES(GOBLET_classifyaggregate_RESOURCES_RCC ${GOBLET_classifyaggregate_RESOURCES})

ADD_EXECUTABLE(goblet-classifyaggregate ${GOBLET_classifyaggregate_SOURCES} ${GOBLET_classifyaggregate_HEADERS} ${GOBLET_classifyaggregate_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifyaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifyaggregate ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-classifyaggregate)
