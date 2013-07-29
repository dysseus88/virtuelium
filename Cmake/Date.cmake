#
# Copyright (C) 2012
#




# Macro --> Get Date
#OS: WIN32 and UNIX
MACRO (TODAY RESULT)
  IF (WIN32)
    EXECUTE_PROCESS(COMMAND "date" "+%Y/%m/%d-%H:%M:%S"
                    OUTPUT_VARIABLE RESULT)
    STRING(STRIP ${RESULT} RESULT) # get rid of newline
  ELSEIF (UNIX)
    EXECUTE_PROCESS(COMMAND "date" "+%Y/%m/%d-%H:%M:%S"
                    OUTPUT_VARIABLE RESULT)
    STRING(STRIP ${RESULT} RESULT) # get rid of newline
  ELSE (WIN32)
    MESSAGE(SEND_ERROR "date not implemented")
    SET(${RESULT} 000000)
  ENDIF (WIN32)
ENDMACRO (TODAY)

