SET(GOBLET_resetdataset_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_resetdataset_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_resetdataset_HEADERS_MOC ${GOBLET_resetdataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_resetdataset_RESOURCES_RCC ${GOBLET_resetdataset_RESOURCES})

ADD_EXECUTABLE(goblet-resetdataset ${GOBLET_resetdataset_SOURCES} ${GOBLET_resetdataset_HEADERS} ${GOBLET_resetdataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-resetdataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-resetdataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-resetdataset)
