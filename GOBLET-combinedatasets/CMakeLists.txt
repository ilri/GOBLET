SET(GOBLET_combinedatasets_SOURCES main.cpp ../common/embdriver.cpp ../common/mydbconn.cpp)
SET(GOBLET_combinedatasets_HEADERS ../common/embdriver.h ../common/mydbconn.h)
QT4_WRAP_CPP(GOBLET_combinedatasets_HEADERS_MOC ${GOBLET_combinedatasets_HEADERS})

QT4_ADD_RESOURCES(GOBLET_combinedatasets_RESOURCES_RCC ${GOBLET_combinedatasets_RESOURCES})

ADD_EXECUTABLE(goblet-combinedatasets ${GOBLET_combinedatasets_SOURCES} ${GOBLET_combinedatasets_HEADERS} ${GOBLET_combinedatasets_HEADERS_MOC})

INCLUDE_DIRECTORIES(../common)

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combinedatasets ${QT_LIBRARIES} ${MYSQL_LIBRARY} ${AIO_LIBRARY} ${CRYPT_LIBRARY} ${PTHREAD_LIBRARY} ${DL_LIBRARY} ${Z_LIBRARY})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combinedatasets ${QT_LIBRARIES} ${MYSQL_LIBRARY})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-combinedatasets)
