
MACRO(add_lemon_files _sources )
    FOREACH (_current_FILE ${ARGN})
      GET_FILENAME_COMPONENT(_in ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_current_FILE} NAME_WE)

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_out}
         COMMAND lemon
         ARGS
         ${_in}
         DEPENDS ${_in}
      )

      SET(${_sources} ${${_sources}} ${_out})
   ENDFOREACH (_current_FILE)
ENDMACRO(add_lemon_files)

MACRO(add_re2c_files _sources )
    FOREACH (_current_FILE ${ARGN})
      GET_FILENAME_COMPONENT(_in ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_current_FILE} NAME_WE)

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.re.c)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_out}
         COMMAND re2c
         ARGS
         ${_in} > ${_out}
         DEPENDS ${_in}
      )

      SET(${_sources} ${${_sources}} ${_out})
   ENDFOREACH (_current_FILE)
ENDMACRO(add_re2c_files)

project(yap)
set(yap_src
    yapBlock.h
    yapBlock.c
    yapContext.h
    yapContext.c
    yapFrame.h
    yapFrame.c
    yapModule.h
    yapModule.c
    yapOp.h
    yapOp.c
    yapTypes.h
    yapTypes.c
    yapValue.h
    yapValue.c
    yapVariable.h
    yapVariable.c
    yapVM.h
    yapVM.c
)
add_lemon_files(yap_src yapTokenizer.y)
add_re2c_files(yap_src yapLexer.re)
add_library(yap ${yap_src})

