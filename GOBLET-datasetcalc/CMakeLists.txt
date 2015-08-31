SET(GOBLET_datasetcalc_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_datasetcalc_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_datasetcalc_HEADERS_MOC ${GOBLET_datasetcalc_HEADERS})

QT4_ADD_RESOURCES(GOBLET_datasetcalc_RESOURCES_RCC ${GOBLET_datasetcalc_RESOURCES})

ADD_EXECUTABLE(goblet-datasetcalc ${GOBLET_datasetcalc_SOURCES} ${GOBLET_datasetcalc_HEADERS} ${GOBLET_datasetcalc_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-datasetcalc ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-datasetcalc ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-datasetcalc)
