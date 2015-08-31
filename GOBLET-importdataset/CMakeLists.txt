SET(GOBLET_importdataset_SOURCES main.cpp gridtocsv.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_importdataset_HEADERS gridtocsv.h ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_importdataset_HEADERS_MOC ${GOBLET_importdataset_HEADERS})

QT4_ADD_RESOURCES(GOBLET_importdataset_RESOURCES_RCC ${GOBLET_importdataset_RESOURCES})

ADD_EXECUTABLE(goblet-importdataset ${GOBLET_importdataset_SOURCES} ${GOBLET_importdataset_HEADERS} ${GOBLET_importdataset_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importdataset ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-importdataset ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-importdataset)
