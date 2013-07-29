################################################################################
# Copyright (c) xxxxxx 2012
# Defines specific macros for this project
################################################################################

################################################################################
# create vs filters
################################################################################
MACRO (cmp_IDE_SOURCE_PROPERTIES SOURCE_PATH FILE_LIST)
  STRING (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH})
  SOURCE_GROUP (${source_group_path} FILES ${FILE_LIST})
ENDMACRO (cmp_IDE_SOURCE_PROPERTIES NAME FILE_LIST INSTALL_FILES)

################################################################################
# set variable and print a message
################################################################################
MACRO (SET_MESSAGE var value)
  SET (${var} ${value})
  MESSAGE (STATUS "${var}: " ${${var}})
ENDMACRO (SET_MESSAGE var value)

################################################################################
# include directory and print a message
################################################################################
MACRO (INCLDIR_MESSAGE inc)
  INCLUDE_DIRECTORIES (${inc})
  MESSAGE (STATUS "Include directory: ${inc}")
ENDMACRO (INCLDIR_MESSAGE inc)

################################################################################
# link directory and print a message
################################################################################
MACRO (LINKDIR_MESSAGE lib)
  LINK_DIRECTORIES (${lib})
  MESSAGE (STATUS "Link directory: ${lib}")
ENDMACRO (LINKDIR_MESSAGE lib)

################################################################################
# List all subdirectories matching  pattern of curdir and stored them in result
################################################################################
MACRO (SUBDIRLIST result curdir pattern)
  FILE (GLOB children RELATIVE ${curdir} ${curdir}/${pattern})
  SET (dirlist "")
  FOREACH(child ${children})
    IF (IS_DIRECTORY ${curdir}/${child})
        SET (dirlist ${dirlist} ${child})
    ENDIF ()
  ENDFOREACH()
  SET (${result} ${dirlist})
ENDMACRO()
################################################################################
# Declare a directory that must be looking for when getting all the source files
#  dir_list is a list of declared directories variables. Use ${...} on these 
#           variables to access to their values
#  dir_pattern is the current pattern used by FILE (GLOB ...) to find the files
#              of the declared directory
################################################################################
MACRO (DECLARE_SRC_DIRECTORY dir_list dir dir_pattern)
  FILE (GLOB ${dir} ${dir_pattern})
  SET (${dir_list} ${${dir_list}} ${dir})
ENDMACRO (DECLARE_SRC_DIRECTORY dir_list dir dir_pattern)

################################################################################
# Get Relative path of a file 
################################################################################
MACRO (GET_RELATIVE_PATH current_path file_path result)
  GET_FILENAME_COMPONENT(PATH_ONLY ${file_path} PATH)
  STRING (LENGTH "${current_path}" LCURRENT) 
  STRING (SUBSTRING ${PATH_ONLY} ${LCURRENT} -1 REL_PATH)
  STRING (SUBSTRING ${REL_PATH} 1 -1 REL_PATH) 
  SET (${result} ${REL_PATH})
ENDMACRO (GET_RELATIVE_PATH current_path file_path result)

################################################################################
# Get Idx of the warning level
################################################################################
MACRO (GET_WARNING_IDX WARNING_LEVEL result)
  IF (${WARNING_LEVEL} MATCHES "No")
    SET (${result} 0)
  ELSEIF (${WARNING_LEVEL} MATCHES "Low")
    SET (${result} 1)
  ELSEIF (${WARNING_LEVEL} MATCHES "Medium")
    SET (${result} 2)
  ELSEIF (${WARNING_LEVEL} MATCHES "High")
    SET (${result} 3)
  ELSEIF (${WARNING_LEVEL} MATCHES "Simple")
    SET (${result} 3)
  ELSEIF (${WARNING_LEVEL} MATCHES "Maximum")
    SET (${result} 4)
  ELSEIF (${WARNING_LEVEL} MATCHES "All")
    SET (${result} 5)
  ENDIF ()  
ENDMACRO (GET_WARNING_IDX WARNING_LEVEL result)

################################################################################
# Update CXX flag for warning level with WFLAG (msvc)
################################################################################
MACRO (UPDATE_MSVC_FLAGS_FOR_WARNING WFLAG WERROR)
    IF (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      STRING (REGEX REPLACE "/W[0-4]" "/W${WFLAG}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    ELSEIF (CMAKE_CXX_FLAGS MATCHES "/Wall")
      STRING (REGEX REPLACE "/Wall" "/W${WFLAG}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    ELSE ()
      SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W${WFLAG}")
    ENDIF ()
    #warnign as error
    IF (${WERROR})
      IF (NOT CMAKE_CXX_FLAGS MATCHES "/WX")
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
      ENDIF()
    ELSE (${WERROR})
      IF (CMAKE_CXX_FLAGS MATCHES "/WX")
       STRING (REGEX REPLACE "/WX" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
     ENDIF()   
    ENDIF (${WERROR})   
ENDMACRO (UPDATE_MSVC_FLAGS_FOR_WARNING WFLAG WERROR)
