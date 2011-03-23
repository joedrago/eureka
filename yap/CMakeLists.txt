
MACRO(add_lemon_files _sources )
    FOREACH (_current_FILE ${ARGN})
      GET_FILENAME_COMPONENT(_in ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_current_FILE} NAME_WE)

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c)
      set(_template ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.lt)

      ADD_CUSTOM_COMMAND(
         OUTPUT ${_out}
         COMMAND lemon
         ARGS
         ${_in}
         DEPENDS ${_in} ${_template}
      )

      SET(${_sources} ${${_sources}} ${_out})
   ENDFOREACH (_current_FILE)
ENDMACRO(add_lemon_files)

MACRO(add_re2c_files _sources )
    FOREACH (_current_FILE ${ARGN})
      GET_FILENAME_COMPONENT(_in ${_current_FILE} ABSOLUTE)
      GET_FILENAME_COMPONENT(_basename ${_current_FILE} NAME_WE)

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.re.inc)
      set(_incd ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c)

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
    yapCode.h
    yapCode.c
    yapCompiler.h
    yapCompiler.c
    yapContext.h
    yapContext.c
    yapObject.h
    yapObject.c
    yapFrame.h
    yapFrame.c
    yapLexer.c
    yapLexer.h
    yapLexer.re
    yapModule.h
    yapModule.c
    yapOp.h
    yapOp.c
    yapParser.y
    yapSyntax.h
    yapSyntax.c
    yapTypes.h
    yapTypes.c
    yapValue.h
    yapValue.c
    yapVariable.h
    yapVariable.c
    yapVM.h
    yapVM.c
    yapxDot.h
    yapxDot.c
    yapxDisasm.h
    yapxDisasm.c
)
add_lemon_files(yap_src yapParser.y)
add_re2c_files(yap_src yapLexer.re)
add_library(yap ${yap_src})

