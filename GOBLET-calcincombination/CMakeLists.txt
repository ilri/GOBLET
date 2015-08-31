SET(GOBLET_calcincombination_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_calcincombination_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_calcincombination_HEADERS_MOC ${GOBLET_calcincombination_HEADERS})

QT4_ADD_RESOURCES(GOBLET_calcincombination_RESOURCES_RCC ${GOBLET_calcincombination_RESOURCES})

ADD_EXECUTABLE(goblet-calcincombination ${GOBLET_calcincombination_SOURCES} ${GOBLET_calcincombination_HEADERS} ${GOBLET_calcincombination_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-calcincombination ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-calcincombination ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-calcincombination)
