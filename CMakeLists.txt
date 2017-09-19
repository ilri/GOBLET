cmake_minimum_required (VERSION 2.6) 

PROJECT(GOBLET)
add_compile_options(-std=c++11)
SET (CMAKE_CXX_FLAGS "-fPIC")

SET(CMAKE_BUILD_TYPE "Release")


SET(CPACK_SOURCE_STRIP_FILES TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Qt5Core REQUIRED) 
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Tclap REQUIRED)
FIND_PACKAGE(shapelib REQUIRED)

SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_DONT_USE_QTGUI TRUE)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})

INCLUDE_DIRECTORIES(${Qt5Xml_INCLUDE_DIRS})
ADD_DEFINITIONS(${Qt5Xml_DEFINITIONS})

INCLUDE_DIRECTORIES(${Qt5Sql_INCLUDE_DIRS})
ADD_DEFINITIONS(${Qt5Sql_DEFINITIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${SHAPELIB_INCLUDE_DIR} ${TCLAP_INCLUDEDIR})

add_subdirectory (GOBLET-createdb)
add_subdirectory (GOBLET-aggregatedataset)
add_subdirectory (GOBLET-calcincombination)
add_subdirectory (GOBLET-classifyaggregate)
add_subdirectory (GOBLET-classifydataset)
add_subdirectory (GOBLET-combineaggregate)
add_subdirectory (GOBLET-combinedatasets)
add_subdirectory (GOBLET-datasetcalc)
add_subdirectory (GOBLET-datasetextract)
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