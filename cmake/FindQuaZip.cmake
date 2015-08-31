FIND_PATH(QUAZIP_INCLUDE_DIR quazip.h
  /usr/include/quazip 
  /usr/local/include/quazip
  /usr/local/gnome/include/quazip
)

SET(QUAZIP_NAMES quazip)
FIND_LIBRARY(QUAZIP_LIBRARY
  NAMES ${QUAZIP_NAMES}
  PATHS /usr/lib /usr/local/lib 
  /usr/lib64
  /usr/local/gnome/lib
)

IF (QUAZIP_LIBRARY)          
    MESSAGE(STATUS "Quazip founded:  ${QUAZIP_LIBRARY}")
    MESSAGE(STATUS "Quazip include:  ${QUAZIP_INCLUDE_DIR}")
ELSE (QUAZIP_LIBRARY)
    MESSAGE(FATAL_ERROR "Quazip library not found") 
ENDIF (QUAZIP_LIBRARY)
