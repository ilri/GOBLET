SET(GOBLET_gendatasetstats_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_gendatasetstats_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_gendatasetstats_HEADERS_MOC ${GOBLET_gendatasetstats_HEADERS})

QT4_ADD_RESOURCES(GOBLET_gendatasetstats_RESOURCES_RCC ${GOBLET_gendatasetstats_RESOURCES})

ADD_EXECUTABLE(goblet-gendatasetstats ${GOBLET_gendatasetstats_SOURCES} ${GOBLET_gendatasetstats_HEADERS} ${GOBLET_gendatasetstats_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-gendatasetstats ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-gendatasetstats ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-gendatasetstats)
