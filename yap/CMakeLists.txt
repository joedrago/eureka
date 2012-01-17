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
         COMMAND ${YAP_RE2C_PATH}
         ARGS
         ${_in} > ${_out}
         DEPENDS ${_in}
      )

      SET(${_sources} ${${_sources}} ${_out})
   ENDFOREACH (_current_FILE)
ENDMACRO(add_re2c_files)

project(yap)
set(yap_src
    core/yapArray.h
    core/yapArray.c
    core/yapBlock.h
    core/yapBlock.c
    core/yapChunk.h
    core/yapChunk.c
    core/yapCode.h
    core/yapCode.c
    core/yapCompiler.h
    core/yapCompiler.c
    core/yapContext.h
    core/yapContext.c
    core/yapObject.h
    core/yapObject.c
    core/yapFrame.h
    core/yapFrame.c
    core/yapHash.h
    core/yapHash.c
    core/yapLexer.c
    core/yapLexer.h
    core/yapLexer.re
    core/yapOp.h
    core/yapOp.c
    core/yapParser.y
    core/yapSyntax.h
    core/yapSyntax.c
    core/yapTypes.h
    core/yapTypes.c
    core/yapValue.h
    core/yapValue.c
    core/yapVariable.h
    core/yapVariable.c
    core/yapVM.h
    core/yapVM.c
)

set(yap_intrinsics
    intrinsics/yapiArray.h
    intrinsics/yapiArray.c
)

set(yap_modules
    modules/yapmAll.c
    modules/yapmAll.h
)

if(YAP_ENABLE_MOD_JSON)
    include_directories(${cjson_SOURCE_DIR})
    set(yap_modules ${yap_modules}
        modules/yapmJSON.c
        modules/yapmJSON.h
    )
endif()

if(YAP_ENABLE_MOD_PCRE)
    include_directories(${PCRE_BINARY_DIR})
    set(yap_modules ${yap_modules}
        modules/yapmPCRE.c
        modules/yapmPCRE.h
    )
endif()

set(yap_extras
)

if(YAP_ENABLE_EXT_DISASM)
    set(yap_extras ${yap_extras}
        extras/yapxDisasm.h
        extras/yapxDisasm.c
    )
endif()

if(YAP_ENABLE_EXT_DOT)
    set(yap_extras ${yap_extras}
        extras/yapxDot.h
        extras/yapxDot.c
    )
endif()

include_directories(core intrinsics modules extras)

add_lemon_files(yap_src core/yapParser.y)
add_re2c_files(yap_src core/yapLexer.re)

add_library(yap
    ${yap_src}
    ${yap_intrinsics}
    ${yap_modules}
    ${yap_extras}
)
