################################################################################
FUNCTION (ADD_AND_INSTALL_LIBRARY name)
  PARSE_ARGUMENTS(ARG "SRC;LIB;DEST" "" ${ARGN})
  ADD_LIBRARY(${name} ${ARG_SRC})
  TARGET_LINK_LIBRARIES(${name} ${ARG_LIB})
  INSTALL(TARGETS ${name} ARCHIVE DESTINATION Lib PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )
ENDFUNCTION (ADD_AND_INSTALL_LIBRARY)

################################################################################
FUNCTION (ADD_AND_INSTALL_EXECUTABLE name)
  PARSE_ARGUMENTS(ARG "SRC;LIB;DEST" "" ${ARGN})
  SET(NAME_EXE_EXT ${name}${EXECUTABLE_EXT})
  ADD_EXECUTABLE(${NAME_EXE_EXT} ${ARG_SRC})
  TARGET_LINK_LIBRARIES(${NAME_EXE_EXT} ${ARG_LIB}) 
  INSTALL(TARGETS ${NAME_EXE_EXT} RUNTIME DESTINATION Bin PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )
ENDFUNCTION (ADD_AND_INSTALL_EXECUTABLE)