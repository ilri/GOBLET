SET(GOBLET_gendatasetstats_SOURCES main.cpp)
QT5_WRAP_CPP(GOBLET_gendatasetstats_HEADERS_MOC)
QT5_ADD_RESOURCES(GOBLET_gendatasetstats_RESOURCES_RCC ${GOBLET_gendatasetstats_RESOURCES})

ADD_EXECUTABLE(goblet-gendatasetstats ${GOBLET_gendatasetstats_SOURCES} ${GOBLET_gendatasetstats_HEADERS_MOC})

IF(NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-gendatasetstats ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ELSE (NOT WIN32)
  TARGET_LINK_LIBRARIES(goblet-gendatasetstats ${Qt5Core_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Sql_LIBRARIES})
ENDIF (NOT WIN32)

INSTALL_PROGRAMS(/bin FILES goblet-gendatasetstats)
