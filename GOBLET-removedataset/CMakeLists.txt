SET(GOBLET_removedataset_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_removedataset_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_removedataset_HEADERS_MOC ${GOBLET_removedataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_removedataset_RESOURCES_RCC ${GOBLET_removedataset_RESOURCES})

ADD_EXECUTABLE(goblet-removedataset ${GOBLET_removedataset_SOURCES} ${GOBLET_removedataset_HEADERS} ${GOBLET_removedataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-removedataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-removedataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-removedataset)
