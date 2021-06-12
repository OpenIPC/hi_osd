cmake_minimum_required(VERSION 2.8)
project(hi_osd C)

if ("${CROSS_COMPILE}" STREQUAL "")
    set(CROSS_COMPILE "arm-himix100-linux-")
endif()

set(CMAKE_AR ${CROSS_COMPILE}ar)
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_LINKER ${CROSS_COMPILE}ld)
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy)
set(CMAKE_RANLIB ${CROSS_COMPILE}ranlib)
set(CMAKE_SIZE ${CROSS_COMPILE}size)
set(CMAKE_STRIP ${CROSS_COMPILE}strip)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#set(CMAKE_LINK_WHAT_YOU_USE "1")
set(CMAKE_C_FLAGS "-std=gnu99")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_PREFIX_PATH ".")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

set(SDLLIBS
#    SDL SDL_ttf freetype
    ${CMAKE_SOURCE_DIR}/lib/libSDL.a
    ${CMAKE_SOURCE_DIR}/lib/libSDL_ttf.a
    ${CMAKE_SOURCE_DIR}/lib/libfreetype.a
)

set(HILIBS
    pthread m dl
    mpi upvqe dnvqe VoiceEngine securec

)

set(HILIBS_CV300
    pthread m dl 
    ${CMAKE_SOURCE_DIR}/lib/cv300/libmpi.a
)

set(SRC
    src/hi_osd.c
    common/loadbmp.c
    src/sdl.c
)

include_directories("../lib/SDL")

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
add_executable(${CMAKE_PROJECT_NAME}_cv300 ${SRC})

target_compile_definitions(${CMAKE_PROJECT_NAME}_cv300  PRIVATE __16CV300__)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE __16EV200__)

target_link_libraries(${CMAKE_PROJECT_NAME} ${HILIBS} ${SDLLIBS})
target_link_libraries(${CMAKE_PROJECT_NAME}_cv300 ${HILIBS_CV300} ${SDLLIBS})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}       POST_BUILD COMMAND "${CMAKE_STRIP}" "hi_osd")
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}_cv300 POST_BUILD COMMAND "${CMAKE_STRIP}" "hi_osd_cv300")

