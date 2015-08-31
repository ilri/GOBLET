# - Find aio library


FIND_PATH(SHAPELIB_INCLUDE_DIR shapefil.h
  /usr/local/include
  /usr/include
  /home/cquiros/data/projects/RMG/software/GOBLET/tools/3rdParty/shapelib
  ~/data/projects/ILRI-MARIO/software/GOBLET/tools/3rdParty/shapelib
)

SET(SHAPELIB_NAMES shp)
FIND_LIBRARY(SHAPELIB_LIBRARY
  NAMES ${SHAPELIB_NAMES}
  PATHS /usr/lib /usr/local/lib /usr/lib64 /home/cquiros/data/projects/RMG/software/GOBLET/tools/3rdParty/shapelib ~/data/projects/ILRI-MARIO/software/GOBLET/tools/3rdParty/shapelib
)

IF (SHAPELIB_LIBRARY)          
    execute_process(COMMAND nm ${SHAPELIB_LIBRARY} COMMAND grep DBFReorderFields  OUTPUT_VARIABLE rv)        
    IF (rv STREQUAL "")
      MESSAGE(FATAL_ERROR "Shapelib library found but not version >= 1.3")
    ENDIF (rv STREQUAL "")
    MESSAGE(STATUS "SHAPELIB >= 1.3 founded:  ${SHAPELIB_LIBRARY}")
    MESSAGE(STATUS "SHAPELIB include:  ${SHAPELIB_INCLUDE_DIR}")
ELSE (SHAPELIB_LIBRARY)
    MESSAGE(FATAL_ERROR "Shapelib library not found") 
ENDIF (SHAPELIB_LIBRARY)