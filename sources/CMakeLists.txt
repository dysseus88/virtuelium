SET (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE (SourceInclusion)
################################################################################
# Get header files
################################################################################
FOREACH (DIR ${GLOBAL_HEADER_FILES})
  SET (CURRENT_PROJECT_HEADERS ${CURRENT_PROJECT_HEADERS} ${${DIR}})
ENDFOREACH (DIR ${GLOBAL_HEADER_FILES})

# MESSAGE (STATUS "Header files:")
# FOREACH (CFILE ${CURRENT_PROJECT_HEADERS})
  # MESSAGE (STATUS "-- ${CFILE}")
# ENDFOREACH (CFILE ${CURRENT_PROJECT_HEADERS})

################################################################################
# Get source files
################################################################################
FOREACH (DIR ${GLOBAL_SOURCE_FILES})
  SET (CURRENT_PROJECT_SOURCES ${CURRENT_PROJECT_SOURCES} ${${DIR}})
ENDFOREACH (DIR ${GLOBAL_SOURCE_FILES})

# MESSAGE (STATUS "Source files:")
# FOREACH (CFILE ${CURRENT_PROJECT_SOURCES})
  # MESSAGE (STATUS "-- ${CFILE}")
# ENDFOREACH (CFILE ${CURRENT_PROJECT_SOURCES})

################################################################################
# Include and link directory
################################################################################
INCLDIR_MESSAGE (${PROJECT_COMMON})
INCLDIR_MESSAGE (${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_DIRNAME})
INCLDIR_MESSAGE (${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIRNAME})

################################################################################
# Add and install library
################################################################################
ADD_AND_INSTALL_EXECUTABLE (${PROJECT_NAME}
                            SRC ${CURRENT_PROJECT_HEADERS}  
                                ${CURRENT_PROJECT_SOURCES}
                            LIB ${LINK_DEPENDENCIES}
                            DEST ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
SET_TARGET_PROPERTIES (${PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${PROJECT_NAME}d)
SET_TARGET_PROPERTIES (${PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${PROJECT_NAME})

IF (WIN32)
  ADD_CUSTOM_COMMAND (TARGET ${PROJECT_NAME}
                      POST_BUILD
                      COMMAND cmd ARGS /k ${PROJECT_BIN}/vs_copy.bat)
  
ELSE (WIN32)
  ADD_CUSTOM_COMMAND (TARGET ${PROJECT_NAME}
                      POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${PROJECT_NAME}
                      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                      COMMENT "Copying binary files to root directory...")
ENDIF (WIN32)

################################################################################
# Create MSVS filters
################################################################################
IF(MSVC)
  MESSAGE (STATUS "Create Visual studio filters...")
  # Headers 
  FOREACH (DIR ${GLOBAL_HEADER_FILES})
    # Create the filter from the relative path without filenames 
    LIST (GET ${DIR} 0 HEAD)
    GET_RELATIVE_PATH (${CMAKE_CURRENT_SOURCE_DIR} ${HEAD} SUBDIR)
    cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR}" "${${DIR}}")    
  ENDFOREACH (DIR ${GLOBAL_HEADER_FILES})
  # Sources 
  FOREACH (DIR ${GLOBAL_SOURCE_FILES})
    # Create the filter from the relative path without filenames 
    LIST (GET ${DIR} 0 HEAD)
    GET_RELATIVE_PATH (${CMAKE_CURRENT_SOURCE_DIR} ${HEAD} SUBDIR)
    cmp_IDE_SOURCE_PROPERTIES( "${SUBDIR}" "${${DIR}}")    
  ENDFOREACH (DIR ${GLOBAL_SOURCE_FILES})
ENDIF(MSVC)

################################################################################
# Install headers
################################################################################
MESSAGE (STATUS "Prepare header files for future installations...")
FOREACH (DIR ${GLOBAL_HEADER_FILES})
  # Get the current file path
  LIST (GET ${DIR} 0 HEAD)
  GET_RELATIVE_PATH (${CMAKE_CURRENT_SOURCE_DIR} ${HEAD} SUBDIR)
  #install 
  INSTALL(DIRECTORY ./${SUBDIR}
          DESTINATION ${PROJECT_NAME}/${SUBDIR}
          FILES_MATCHING PATTERN "*.hpp")
ENDFOREACH (DIR ${GLOBAL_HEADER_FILES})
