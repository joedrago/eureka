
project(yapgame C)

FIND_LIBRARY(GL_LIBRARY OpenGL)
MARK_AS_ADVANCED(GL_LIBRARY)
FIND_LIBRARY(GLUT_LIBRARY GLUT)
MARK_AS_ADVANCED(GLUT_LIBRARY)
FIND_LIBRARY(CF_LIBRARY CoreFoundation)
MARK_AS_ADVANCED(CF_LIBRARY)
FIND_LIBRARY(CARBON_LIBRARY Carbon)
MARK_AS_ADVANCED(CARBON_LIBRARY)

if(UNIX)
#    add_definitions(-DHAVE_READLINE=1)
endif()

include_directories (${yap_SOURCE_DIR}
    /System/Library/Frameworks/GLUT.Framework/Headers
    /System/Library/Frameworks/OpenGL.Framework/Versions/A/Headers
)
add_executable(yapgame
    main.c
)
target_link_libraries (yapgame yap
    ${CF_LIBRARY}
    ${CARBON_LIBRARY}
    ${GL_LIBRARY}
    ${GLUT_LIBRARY}
)

