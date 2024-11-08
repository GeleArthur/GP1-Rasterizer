cmake_minimum_required(VERSION 3.20..3.31)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# set(CMAKE_OSX_ARCHITECTURES "x86_64")
# set(CMAKE_SUPPRESS_REGENERATION true)

# Source files
set(SOURCES 
    "src/main.cpp"
    "src/Camera.h"
    "src/Matrix.cpp"
    "src/Renderer.cpp"
	"src/Texture.cpp"
    "src/Timer.cpp"
	"src/Vector2.cpp"
    "src/Vector3.cpp"
    "src/Vector4.cpp"
    "src/ColorRGB.h"
    "src/DataTypes.h"
    "src/MathHelpers.h"
    "src/Maths.h"
    "src/Matrix.h"
    "src/Renderer.h"
    "src/Texture.h"
    "src/Timer.h"
    "src/Vector2.h"
    "src/Vector3.h"
    "src/Vector4.h"
    "src/Utils.h"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})




find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)


target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${PROJECT_SOURCE_DIR}/Resources"
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
