SET(GOBLET_removedataset_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_removedataset_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_removedataset_RESOURCES_RCC ${GOBLET_removedataset_RESOURCES})

ADD_EXECUTABLE(goblet-removedataset ${GOBLET_removedataset_SOURCES} ${GOBLET_removedataset_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-removedataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-removedataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-removedataset)
