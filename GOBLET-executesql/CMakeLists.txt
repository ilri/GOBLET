SET(GOBLET_executesql_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_executesql_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_executesql_HEADERS_MOC ${GOBLET_executesql_HEADERS})

QT4_ADD_RESOURCES(GOBLET_executesql_RESOURCES_RCC ${GOBLET_executesql_RESOURCES})

ADD_EXECUTABLE(goblet-executesql ${GOBLET_executesql_SOURCES} ${GOBLET_executesql_HEADERS} ${GOBLET_executesql_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-executesql ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-executesql ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-executesql)
