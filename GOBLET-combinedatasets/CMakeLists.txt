SET(GOBLET_combinedatasets_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_combinedatasets_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_combinedatasets_RESOURCES_RCC ${GOBLET_combinedatasets_RESOURCES})

ADD_EXECUTABLE(goblet-combinedatasets ${GOBLET_combinedatasets_SOURCES} ${GOBLET_combinedatasets_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combinedatasets ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-combinedatasets ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-combinedatasets)
