SET(GOBLET_modifydataset_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_modifydataset_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_modifydataset_RESOURCES_RCC ${GOBLET_modifydataset_RESOURCES})

ADD_EXECUTABLE(goblet-modifydataset ${GOBLET_modifydataset_SOURCES} ${GOBLET_modifydataset_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-modifydataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-modifydataset ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-modifydataset)
