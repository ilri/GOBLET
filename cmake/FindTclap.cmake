# - Find aio library


FIND_PATH(TCLAP_INCLUDEDIR "tclap/CmdLine.h"
  /usr/local/include/tclap
  /usr/include/tclap
  /home/cquiros/data/projects/RMG/software/GOBLET/tools/3rdParty
)
IF(TCLAP_INCLUDEDIR)
  SET(TCLAP_FOUND TRUE)
  MESSAGE(STATUS "Tclap founded:  ${TCLAP_INCLUDEDIR}")
ELSE(TCLAP_INCLUDEDIR)
  SET(TCLAP_FOUND FALSE)
  MESSAGE(FATAL_ERROR "Tclap library not found")
ENDIF()