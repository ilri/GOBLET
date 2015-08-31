cmake_minimum_required (VERSION 2.6) 

PROJECT(GOBLET)

SET(CMAKE_BUILD_TYPE "Release")

SET(CPACK_SOURCE_STRIP_FILES TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MySQLEmbedded REQUIRED)
FIND_PACKAGE(Tclap REQUIRED)

IF(NOT WIN32)
  FIND_PACKAGE(aio REQUIRED)
  FIND_PACKAGE(crypt REQUIRED)
  FIND_PACKAGE(pthread REQUIRED)
  FIND_PACKAGE(dl REQUIRED)
  FIND_PACKAGE(rt REQUIRED)
  FIND_PACKAGE(QuaZip REQUIRED)
  FIND_PACKAGE(z REQUIRED)
ENDIF(NOT WIN32)

FIND_PACKAGE(shapelib REQUIRED)

SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_DONT_USE_QTGUI TRUE)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${MYSQL_INCLUDE_DIR} ${SHAPELIB_INCLUDE_DIR} ${TCLAP_INCLUDEDIR} ${QUAZIP_INCLUDE_DIR})

add_subdirectory (GOBLET-createdb)
add_subdirectory (GOBLET-aggregatedataset)
add_subdirectory (GOBLET-calcincombination)
add_subdirectory (GOBLET-classifyaggregate)
add_subdirectory (GOBLET-classifydataset)
add_subdirectory (GOBLET-combineaggregate)
add_subdirectory (GOBLET-combinedatasets)
add_subdirectory (GOBLET-datasetcalc)
add_subdirectory (GOBLET-describeshape)
add_subdirectory (GOBLET-genaggregatestats)
add_subdirectory (GOBLET-gendatasetstats)
add_subdirectory (GOBLET-listdatasets)
add_subdirectory (GOBLET-modifydataset)
add_subdirectory (GOBLET-outputraster)
add_subdirectory (GOBLET-resetaggregate)
add_subdirectory (GOBLET-resetdataset)
add_subdirectory (GOBLET-outputshape)
add_subdirectory (GOBLET-importshape)
add_subdirectory (GOBLET-importdataset)
add_subdirectory (GOBLET-removedataset)
add_subdirectory (GOBLET-executesql)
add_subdirectory (GOBLET-aggregatetoshape)
add_subdirectory (GOBLET-calcinshape)
add_subdirectory (GOBLET-extractmetadata)