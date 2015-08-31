SET(GOBLET_createdb_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_createdb_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_createdb_HEADERS_MOC ${GOBLET_createdb_HEADERS})
SET(GOBLET_createdb_RESOURCES mysqlFiles.qrc)

QT4_ADD_RESOURCES(GOBLET_createdb_RESOURCES_RCC ${GOBLET_createdb_RESOURCES})

ADD_EXECUTABLE(goblet-createdb ${GOBLET_createdb_SOURCES} ${GOBLET_createdb_HEADERS} ${GOBLET_createdb_HEADERS_MOC} ${GOBLET_createdb_RESOURCES_RCC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-createdb ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${QUAZIP_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-createdb ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-createdb)
