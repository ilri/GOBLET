SET(GOBLET_extractmetadata_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_extractmetadata_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_extractmetadata_HEADERS_MOC ${GOBLET_extractmetadata_HEADERS})

QT4_ADD_RESOURCES(GOBLET_extractmetadata_RESOURCES_RCC ${GOBLET_extractmetadata_RESOURCES})

ADD_EXECUTABLE(goblet-extractmetadata ${GOBLET_extractmetadata_SOURCES} ${GOBLET_extractmetadata_HEADERS} ${GOBLET_extractmetadata_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-extractmetadata ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-extractmetadata ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-extractmetadata)
