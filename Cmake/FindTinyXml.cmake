# Try to find TinyXml
# This module defines
# TinyXml_FOUND: if false, do not try to link
# TinyXml_LIBRARY: full path to the library
# TinyXml_INCLUDE_DIR: where to find the headers
FIND_PATH (TinyXml_INCLUDE_DIR
           NAMES TinyXml.hpp
           PATHS ${PROJECT_THIRD}/TinyXml/include)
FIND_LIBRARY (TinyXml_LIBRARY
              NAMES TinyXml
              PATHS ${PROJECT_THIRD}/TinyXml/lib)
SET (TinyXml_FOUND "NO")
IF (TinyXml_LIBRARY AND TinyXml_INCLUDE_DIR)
    SET (TinyXml_FOUND "YES")
ENDIF (TinyXml_LIBRARY AND TinyXml_INCLUDE_DIR)
