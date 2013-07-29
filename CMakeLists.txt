CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

################################################################################
# Customize here
################################################################################
SET (PROJECT_NAME 	  virtuelium)
SET (SOURCE_NAME 	    sources)
SET (HEADER_DIRNAME 	include)
SET (SOURCE_DIRNAME 	src)

################################################################################
# Cmake build type - some IDEs like MsVC will ignore it
################################################################################
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING 
       "Choose the type of build, options are: None(CMAKE_CXX_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING 
       "Choose the type of build, options are: None(CMAKE_CXX_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

################################################################################
# Fix input path definition 
################################################################################
PROJECT (${PROJECT_NAME})
SET (PROJECT_CMAKE    "${CMAKE_CURRENT_SOURCE_DIR}/Cmake")
SET (PROJECT_COMMON   "${CMAKE_CURRENT_SOURCE_DIR}/Common")
SET (PROJECT_THIRD    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParties")
SET (PROJECT_BIN      "${CMAKE_CURRENT_SOURCE_DIR}/Bin")

################################################################################
# See Cmake/version.cmake and Cmake/ProjectMacros.cmake
################################################################################
MESSAGE (STATUS "----------------------------------------")
SET (CMAKE_MODULE_PATH "${PROJECT_CMAKE}")
INCLUDE (Version)
INCLUDE (ProjectMacros)
MESSAGE (STATUS "CMake configuration: ${PROJECT_NAME}")
MESSAGE (STATUS "Version ${${PROJECT_NAME}_VERSION}")
MESSAGE (STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

################################################################################
# See Cmake/ParseArguments.cmake, Cmake/LinkManager.cmake
################################################################################
INCLUDE (ParseArguments)
INCLUDE (LinkManager)

################################################################################
# Output path definition 
################################################################################
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

################################################################################
# Scan and sort directories between :
#   _ Reserved directories (Cmake, Common, Doc, Bin, ThirdParties, SOURCE_NAME)
#   _ Test directories (matching Test*) that may build executables
#   _ Data directories (all others dirrectories) that may be copied
################################################################################                   
SUBDIRLIST (DATA_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} "*")                      
LIST (REMOVE_ITEM DATA_DIRECTORIES "Cmake")
LIST (REMOVE_ITEM DATA_DIRECTORIES "Common")
LIST (REMOVE_ITEM DATA_DIRECTORIES "Doc")
LIST (REMOVE_ITEM DATA_DIRECTORIES "Bin")
LIST (REMOVE_ITEM DATA_DIRECTORIES "ThirdParties")
LIST (REMOVE_ITEM DATA_DIRECTORIES "${SOURCE_NAME}")

################################################################################
# Options for building documentations
################################################################################
OPTION (${PROJECT_NAME}_BUILD_DOCS 				
        "Build ${PROJECT_NAME} documentation in build directory" 
        OFF)

################################################################################
# Options for floating point precision
################################################################################
SET(${PROJECT_NAME}_PRECISION 
    "Float" 
    CACHE STRING "Select simple or double precision")
SET_PROPERTY(CACHE ${PROJECT_NAME}_PRECISION PROPERTY STRINGS Float Double)

SET (type_list_name    ${PROJECT_NAME}_type_list)

IF (${${PROJECT_NAME}_PRECISION} MATCHES "Float")
  LIST (APPEND ${type_list_name} USE_FLOAT)
ELSE (${${PROJECT_NAME}_PRECISION} MATCHES "Float")
  IF (${${PROJECT_NAME}_PRECISION} MATCHES "Double")
    LIST (APPEND ${type_list_name} USE_DOUBLE)
  ENDIF (${${PROJECT_NAME}_PRECISION} MATCHES "Double")
ENDIF (${${PROJECT_NAME}_PRECISION} MATCHES "Float")

SET_PROPERTY (DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
              ${${type_list_name}})

IF (${type_list_name})
  MESSAGE (STATUS "Floating Point Precision: ${${type_list_name}}")
ENDIF (${type_list_name})

################################################################################
# Options for Architecture - windows only : select the right third party path
################################################################################
IF (WIN32)
  SET(${PROJECT_NAME}_ARCHI "win64" CACHE STRING "Select a third party architecture. Warning: due to MPICH2 library, heterogeneous executions are not allowed !")
  SET_PROPERTY(CACHE ${PROJECT_NAME}_ARCHI PROPERTY STRINGS Win32 Win64)

  MESSAGE (STATUS "Windows architecture : ${${PROJECT_NAME}_ARCHI}")
ENDIF (WIN32)

################################################################################
# Specific Settings for MSVC compiler
################################################################################
IF (MSVC) 
  # Build type
  MESSAGE(STATUS "Build Type: Unknown (Choose one in Visual Studio Interface).")
      
  # Warning level : set options to allow users to manage warning level
  #   ${PROJECT_NAME}_WARNING: List of allowed levels
  #   ${PROJECT_NAME}_WERROR: Consider warnings as errors
  SET(${PROJECT_NAME}_WARNING "High" CACHE STRING 
      "Warning Level: None, Low, Medium, High, Maximum, All")
  SET_PROPERTY(CACHE ${PROJECT_NAME}_WARNING 
              PROPERTY STRINGS No Low Medium High Maximum All)
  MESSAGE (STATUS "Warning Level: ${${PROJECT_NAME}_WARNING}")

  # warnings as errors
  OPTION (${PROJECT_NAME}_WERROR "Treat warnings as errors" OFF)
  MESSAGE (STATUS "Warning As Error: ${${PROJECT_NAME}_WERROR}")

 # Warning level : Get idx of the current level, then update compiler flags
  GET_WARNING_IDX (${${PROJECT_NAME}_WARNING} WARNING_LEVEL)
  IF (${WARNING_LEVEL} MATCHES "[0-4]")
    UPDATE_MSVC_FLAGS_FOR_WARNING (${WARNING_LEVEL} 
                                   ${${PROJECT_NAME}_WERROR}) 
  ELSEIF (${WARNING_LEVEL} MATCHES "5")
    UPDATE_MSVC_FLAGS_FOR_WARNING (all 
                                   ${${PROJECT_NAME}_WERROR}) 
  ENDIF ()
  
  # SCL options
  SET_PROPERTY (DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
                _SCL_SECURE_NO_WARNINGS  _SCL_SECURE_NO_DEPRECATE
                _CRT_SECURE_NO_WARNINGS  _CRT_SECURE_NO_DEPRECATE)
  OPTION (${PROJECT_NAME}_USE_SECURE_SCL
          "If not SET, define _SECURE_SCL=0 in release mode for all projects.
          WARNING: ABI incompatible" OFF)
  IF (${PROJECT_NAME}_USE_SECURE_SCL)
    SET_PROPERTY(DIRECTORY APPEND PROPERTY
                 COMPILE_DEFINITIONS_RELEASE "_SECURE_SCL=0")
  ENDIF (${PROJECT_NAME}_USE_SECURE_SCL)

################################################################################
# Setting for other compilers
################################################################################
ELSE (MSVC)
  # Build type
  MESSAGE(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

########## GCC compiler ########################################################
  IF (CMAKE_COMPILER_IS_GNUCXX)
    SET (WARNING_LEVEL "")

    # Standard warnings
    OPTION (${PROJECT_NAME}_WALL "Show all standard warnings (-Wall)" ON)
    MESSAGE (STATUS "All standard warnings: ${${PROJECT_NAME}_WALL}")
    IF (${${PROJECT_NAME}_WALL})
      SET (WARNING_LEVEL "${WARNING_LEVEL} -Wall")
    ENDIF ()
    
    # Standard warnings
    OPTION (${PROJECT_NAME}_WEXTRA "Show extra warnings (-Wextra)" OFF)
    MESSAGE (STATUS "Extra warnings: ${${PROJECT_NAME}_WEXTRA}")
    IF (${${PROJECT_NAME}_WEXTRA})
      SET (WARNING_LEVEL "${WARNING_LEVEL} -Wextra")
    ENDIF ()
   
    # Standard warnings
    OPTION (${PROJECT_NAME}_WPEDANTIC "Show ISO C/C++ warnings (-pedantic)" OFF)
    MESSAGE (STATUS "ISO C/C++ warnings: ${${PROJECT_NAME}_WPEDANTIC}")
    IF (${${PROJECT_NAME}_WPEDANTIC})
      SET (WARNING_LEVEL "${WARNING_LEVEL} -pedantic")
    ENDIF ()
   
    # warnings as errors
    OPTION (${PROJECT_NAME}_WERROR "Treat warnings as errors (-Werror)" OFF)
    MESSAGE (STATUS "Warnings As Error: ${${PROJECT_NAME}_WERROR}")
    IF (${${PROJECT_NAME}_WERROR})
      SET (WARNING_LEVEL "${WARNING_LEVEL} -Werror")
    ENDIF ()
           
    # fill and print cxx flags
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_LEVEL}")
    MESSAGE (STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    
########## Other compiler ######################################################
  ELSE ()
    MESSAGE ("This compiler cannot be properly configured yet.")
    MESSAGE ("Please, use ${PROJECT_NAME}_CXX_FLAGS to manually configure it.")
  
    SET (${PROJECT_NAME}_CXX_FLAGS "" CACHE STRING 
      "Additionnal CXX flags for this compiler.")

    # fill and print cxx flags
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${PROJECT_NAME}_CXX_FLAGS}")
  ENDIF ()    
ENDIF (MSVC)

################################################################################
# Begining building processes
################################################################################
MESSAGE (STATUS "----------------------------------------")

################################################################################
# Dependencies: libdevIL
################################################################################
IF (WIN32)
  SET (DEVIL_INCLUDE ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/include)
  SET (DEVIL_LIBRARY ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/DevIL.lib;
                     ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/ILU.lib;
                     ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/ILUT.lib)
  INSTALL (FILES ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/DevIL.dll 
           DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
  INSTALL (FILES ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/ILU.dll 
           DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
  INSTALL (FILES ${PROJECT_THIRD}/devIL/${${PROJECT_NAME}_ARCHI}/lib/ILUT.dll 
           DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

ELSE (WIN32)
  FIND_PACKAGE (DevIL REQUIRED)
  IF (IL_FOUND)
    SET (DEVIL_INCLUDE ${IL_INCLUDE_DIR})
    SET (DEVIL_LIBRARY ${IL_LIBRARIES};${ILU_LIBRARIES};${ILUT_LIBRARIES}) 
  ENDIF (IL_FOUND)
ENDIF (WIN32)

INCLDIR_MESSAGE ("${DEVIL_INCLUDE}")

################################################################################
# Dependencies: mpich2
################################################################################
FIND_PACKAGE (MPI REQUIRED)
IF (MPI_CXX_FOUND)
  SET (MPICH2_INCLUDE ${MPI_CXX_INCLUDE_PATH})
  STRING(REGEX REPLACE "lib " "lib;" 
         MPICH2_LIBRARY "${MPI_CXX_LIBRARIES}")
ENDIF (MPI_CXX_FOUND)

MESSAGE(${MPICH2_LIBRARY})

INCLDIR_MESSAGE ("${MPICH2_INCLUDE}")

################################################################################
# Dependencies: openMP (only compile flags)
################################################################################
FIND_PACKAGE (OpenMP REQUIRED)
IF (OPENMP_FOUND)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF (OPENMP_FOUND)

#################################################################################
## Dependencies: boost
#################################################################################
#IF (MSVC) 
#  ADD_DEFINITIONS (-DBOOST_ALL_NO_LIB)
#  SET (Boost_USE_STATIC_LIBS        OFF)
#  SET (BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries
#ENDIF (MSVC) 
#FIND_PACKAGE(Boost)
#IF (Boost_FOUND)
#  INCLDIR_MESSAGE (${Boost_INCLUDE_DIR})
#  LINKDIR_MESSAGE (${Boost_LIBRARY_DIRS})
#  SET (BOOST_LIBRARY ${Boost_LIBRARIES})
#ENDIF (Boost_FOUND)

################################################################################
# Dependencies: tclap (command line argument parser)
################################################################################
INCLDIR_MESSAGE ("${PROJECT_THIRD}/tclap_1.2.1")

################################################################################
# Dependencies: summary
################################################################################
SET (LINK_DEPENDENCIES ${MPICH2_LIBRARY};${DEVIL_LIBRARY};${BOOST_LIBRARY})
MESSAGE (STATUS "LINK_DEPENDENCIES: ${LINK_DEPENDENCIES}")
MESSAGE (STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

################################################################################
# Main Generation
################################################################################
MESSAGE (STATUS "Generate ${PROJECT_NAME} project from source files...")
ADD_SUBDIRECTORY (${SOURCE_NAME})
MESSAGE (STATUS "----------------------------------------")

################################################################################
# Data copying
################################################################################
# FOREACH (DATA ${DATA_LIST})
  # IF (${PROJECT_NAME}_COPY_${DATA})
    # MESSAGE (STATUS "Copy ${DATA} directory...")
    # ADD_SUBDIRECTORY (${DATA})
    # MESSAGE (STATUS "----------------------------------------")    
  # ENDIF (${PROJECT_NAME}_COPY_${DATA})
# ENDFOREACH (DATA ${DATA_LIST})

################################################################################
# Documentation
################################################################################
SET (${PROJECT_NAME}_DOC_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Doc)
SET (${PROJECT_NAME}_DOC_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/Doc)
#important files
SET (DOC_FILES	
    ${${PROJECT_NAME}_DOC_SRC_PATH}/readme.txt
    ${${PROJECT_NAME}_DOC_SRC_PATH}/copyright.txt)
INSTALL(FILES ${DOC_FILES} 
        DESTINATION ${PROJECT_NAME}/Doc)

IF (${PROJECT_NAME}_BUILD_INSTALLER)
  SET (COPYRIGHT_PATH ${${PROJECT_NAME}_DOC_SRC_PATH})
  INCLUDE (UseCPackConfig)
ENDIF (${PROJECT_NAME}_BUILD_INSTALLER)

IF (${PROJECT_NAME}_BUILD_DOCS)
  ADD_SUBDIRECTORY (Doc)
ENDIF (${PROJECT_NAME}_BUILD_DOCS)
