# - Find aio library


FIND_PATH(TCLAP_INCLUDEDIR "tclap/CmdLine.h"
  /usr/include
  /usr/local/include
)
IF(TCLAP_INCLUDEDIR)
  SET(TCLAP_FOUND TRUE)
  MESSAGE(STATUS "Tclap founded:  ${TCLAP_INCLUDEDIR}")
ELSE(TCLAP_INCLUDEDIR)
  SET(TCLAP_FOUND FALSE)
  MESSAGE(FATAL_ERROR "Tclap library not found")
ENDIF()