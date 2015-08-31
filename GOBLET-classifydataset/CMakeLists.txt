SET(GOBLET_classifydataset_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_classifydataset_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_classifydataset_HEADERS_MOC ${GOBLET_classifydataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_classifydataset_RESOURCES_RCC ${GOBLET_classifydataset_RESOURCES})

ADD_EXECUTABLE(goblet-classifydataset ${GOBLET_classifydataset_SOURCES} ${GOBLET_classifydataset_HEADERS} ${GOBLET_classifydataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifydataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-classifydataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-classifydataset)
