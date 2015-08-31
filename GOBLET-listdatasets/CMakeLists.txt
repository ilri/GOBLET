SET(GOBLET_listdatasets_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_listdatasets_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_listdatasets_HEADERS_MOC ${GOBLET_listdatasets_HEADERS})

QT4_ADD_RESOURCES(GOBLET_listdatasets_RESOURCES_RCC ${GOBLET_listdatasets_RESOURCES})

ADD_EXECUTABLE(goblet-listdatasets ${GOBLET_listdatasets_SOURCES} ${GOBLET_listdatasets_HEADERS} ${GOBLET_listdatasets_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-listdatasets ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-listdatasets ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-listdatasets)
