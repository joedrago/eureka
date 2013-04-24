# ---------------------------------------------------------------------------
#                   Copyright Joe Drago 2010 - 2011.
#         Distributed under the Boost Software License, Version 1.0.
#            (See accompanying file LICENSE_1_0.txt or copy at
#                  http:#www.boost.org/LICENSE_1_0.txt)
# ---------------------------------------------------------------------------

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
         COMMAND ${EUREKA_RE2C_PATH}
         ARGS
         -o ${_out} ${_in}
         DEPENDS ${_in}
      )

      SET(${_sources} ${${_sources}} ${_out})
   ENDFOREACH (_current_FILE)
ENDMACRO(add_re2c_files)

set(ek_src
    ekArray.h
    ekArray.c
    ekBlock.h
    ekBlock.c
    ekChunk.h
    ekChunk.c
    ekCode.h
    ekCode.c
    ekCompiler.h
    ekCompiler.c
    ekContext.h
    ekContext.c
    ekObject.h
    ekObject.c
    ekFrame.h
    ekFrame.c
    ekIntrinsics.h
    ekIntrinsics.c
    ekLexer.c
    ekLexer.h
    ekLexer.re
    ekMap.h
    ekMap.c
    ekOp.h
    ekOp.c
    ekParser.y
    ekString.h
    ekString.c
    ekSyntax.h
    ekSyntax.c
    ekTypes.h
    ekTypes.c
    ekValue.h
    ekValue.c
    ekValueType.h
    ekValueType.c
    ekValueTypeArray.c
    ekValueTypeBlock.c
    ekValueTypeCFunction.c
    ekValueTypeFloat.c
    ekValueTypeInt.c
    ekValueTypeNull.c
    ekValueTypeObject.c
    ekValueTypeRef.c
    ekValueTypeString.c
)

set(ek_modules
    ekmAll.c
    ekmAll.h
)

if(EUREKA_ENABLE_MOD_JSON)
    include_directories(${cjson_SOURCE_DIR})
    set(ek_modules ${ek_modules}
        ekmJSON.c
        ekmJSON.h
    )
endif()

if(EUREKA_ENABLE_MOD_PCRE)
    include_directories(${PCRE_BINARY_DIR})
    set(ek_modules ${ek_modules}
        ekmPCRE.c
        ekmPCRE.h
    )
endif()

if(EUREKA_ENABLE_MOD_FILE)
    set(ek_modules ${ek_modules}
        ekmFile.c
        ekmFile.h
    )
endif()

set(ek_extras
)

if(EUREKA_ENABLE_EXT_DISASM)
    set(ek_extras ${ek_extras}
        ekxDisasm.h
        ekxDisasm.c
    )
endif()

if(EUREKA_ENABLE_EXT_DOT)
    set(ek_extras ${ek_extras}
        ekxDot.h
        ekxDot.c
    )
endif()

add_lemon_files(ek_src ekParser.y)
add_re2c_files(ek_src ekLexer.re)

add_library(ek
    ${ek_src}
    ${ek_modules}
    ${ek_extras}
)
