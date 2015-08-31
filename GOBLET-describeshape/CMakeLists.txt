SET(GOBLET_describeshape_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_describeshape_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_describeshape_HEADERS_MOC ${GOBLET_describeshape_HEADERS})

QT4_ADD_RESOURCES(GOBLET_describeshape_RESOURCES_RCC ${GOBLET_describeshape_RESOURCES})

ADD_EXECUTABLE(goblet-describeshape ${GOBLET_describeshape_SOURCES} ${GOBLET_describeshape_HEADERS} ${GOBLET_describeshape_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-describeshape ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${RT_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-describeshape ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-describeshape)
