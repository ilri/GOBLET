SET(GOBLET_aggregatedataset_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_aggregatedataset_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_aggregatedataset_HEADERS_MOC ${GOBLET_aggregatedataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_aggregatedataset_RESOURCES_RCC ${GOBLET_aggregatedataset_RESOURCES})

ADD_EXECUTABLE(goblet-aggregatedataset ${GOBLET_aggregatedataset_SOURCES} ${GOBLET_aggregatedataset_HEADERS} ${GOBLET_aggregatedataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatedataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-aggregatedataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-aggregatedataset)
