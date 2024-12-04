cmake_minimum_required(VERSION 3.20..3.31)

# include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# set(CMAKE_OSX_ARCHITECTURES "x86_64")
# set(CMAKE_SUPPRESS_REGENERATION true)

# Source files
set(SOURCES 
    "src/main.cpp"
    "src/Camera.h"
    "src/Renderer.cpp"
    "src/Texture.cpp"
    "src/Timer.cpp"
    "src/ColorRGB.h"
    "src/DataTypes.h"
    "src/MathHelpers.h"
    "src/Maths.h"
    "src/Matrix.h"
    "src/Renderer.h"
    "src/Texture.h"
    "src/Timer.h"
    "src/Utils.h"
    "src/Vector.h"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Simple Directmedia Layer
set(SDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.30.7")
add_library(SDL STATIC IMPORTED)
set_target_properties(SDL PROPERTIES
    IMPORTED_LOCATION "${SDL_DIR}/lib/x64/SDL2.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL)

file(GLOB_RECURSE DLL_FILES
    "${SDL_DIR}/lib/x64/*.dll"
    "${SDL_DIR}/lib/x64/*.manifest"
)

foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach(DLL)

set(SDL_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image-2.8.2")
add_library(SDL_IMAGE STATIC IMPORTED)
set_target_properties(SDL_IMAGE PROPERTIES
    IMPORTED_LOCATION "${SDL_IMAGE_DIR}/lib/x64/SDL2_image.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_IMAGE_DIR}/include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL_IMAGE)

file(GLOB_RECURSE DLL_FILES
    "${SDL_IMAGE_DIR}/lib/x64/*.dll"
    "${SDL_IMAGE_DIR}/lib/x64/*.manifest"
)

foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach(DLL)


# find_package(SDL2 CONFIG REQUIRED)
# find_package(SDL2_image CONFIG REQUIRED)


# target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
# target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E create_symlink
       "${PROJECT_SOURCE_DIR}/project/Resources"
       "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
)


# Visual Leak Detector
if(WIN32 AND CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(ENABLE_VLD=1)
    set(VLD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/vld")
    add_library(vld STATIC IMPORTED)
    set_target_properties(vld PROPERTIES
        IMPORTED_LOCATION "${VLD_DIR}/lib/x64/vld.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${VLD_DIR}/include"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE vld)

    set(DLL_SOURCE_DIR "${VLD_DIR}/lib")

    file(GLOB_RECURSE DLL_FILES
        "${DLL_SOURCE_DIR}/*.dll"
        "${DLL_SOURCE_DIR}/*.manifest"
    )

    foreach(DLL ${DLL_FILES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach(DLL)
endif()
