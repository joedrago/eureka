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

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/core/${_basename}.c)
      set(_template ${CMAKE_CURRENT_SOURCE_DIR}/core/${_basename}.lt)

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

      set(_out ${CMAKE_CURRENT_SOURCE_DIR}/core/${_basename}.re.inc)
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

project(eureka)
set(ek_src
    core/ekArray.h
    core/ekArray.c
    core/ekBlock.h
    core/ekBlock.c
    core/ekChunk.h
    core/ekChunk.c
    core/ekCode.h
    core/ekCode.c
    core/ekCompiler.h
    core/ekCompiler.c
    core/ekContext.h
    core/ekContext.c
    core/ekObject.h
    core/ekObject.c
    core/ekFrame.h
    core/ekFrame.c
    core/ekLexer.c
    core/ekLexer.h
    core/ekLexer.re
    core/ekMap.h
    core/ekMap.c
    core/ekOp.h
    core/ekOp.c
    core/ekParser.y
    core/ekString.h
    core/ekString.c
    core/ekSyntax.h
    core/ekSyntax.c
    core/ekTypes.h
    core/ekTypes.c
    core/ekValue.h
    core/ekValue.c
    core/ekValueTypeArray.c
    core/ekValueTypeBlock.c
    core/ekValueTypeCFunction.c
    core/ekValueTypeFloat.c
    core/ekValueTypeInt.c
    core/ekValueTypeNull.c
    core/ekValueTypeObject.c
    core/ekValueTypeRef.c
    core/ekValueTypeString.c
)

set(ek_intrinsics
    intrinsics/ekiCore.h
    intrinsics/ekiCore.c
    intrinsics/ekiConversions.h
    intrinsics/ekiConversions.c
    intrinsics/ekiInheritance.h
    intrinsics/ekiInheritance.c
)

set(ek_modules
    modules/ekmAll.c
    modules/ekmAll.h
)

if(EUREKA_ENABLE_MOD_JSON)
    include_directories(${cjson_SOURCE_DIR})
    set(ek_modules ${ek_modules}
        modules/ekmJSON.c
        modules/ekmJSON.h
    )
endif()

if(EUREKA_ENABLE_MOD_PCRE)
    include_directories(${PCRE_BINARY_DIR})
    set(ek_modules ${ek_modules}
        modules/ekmPCRE.c
        modules/ekmPCRE.h
    )
endif()

set(ek_extras
)

if(EUREKA_ENABLE_EXT_DISASM)
    set(ek_extras ${ek_extras}
        extras/ekxDisasm.h
        extras/ekxDisasm.c
    )
endif()

if(EUREKA_ENABLE_EXT_DOT)
    set(ek_extras ${ek_extras}
        extras/ekxDot.h
        extras/ekxDot.c
    )
endif()

include_directories(core intrinsics modules extras)

add_lemon_files(ek_src core/ekParser.y)
add_re2c_files(ek_src core/ekLexer.re)

add_library(eureka
    ${ek_src}
    ${ek_intrinsics}
    ${ek_modules}
    ${ek_extras}
)
