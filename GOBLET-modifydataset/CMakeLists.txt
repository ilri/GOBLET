SET(GOBLET_modifydataset_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_modifydataset_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_modifydataset_HEADERS_MOC ${GOBLET_modifydataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_modifydataset_RESOURCES_RCC ${GOBLET_modifydataset_RESOURCES})

ADD_EXECUTABLE(goblet-modifydataset ${GOBLET_modifydataset_SOURCES} ${GOBLET_modifydataset_HEADERS} ${GOBLET_modifydataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-modifydataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-modifydataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-modifydataset)
