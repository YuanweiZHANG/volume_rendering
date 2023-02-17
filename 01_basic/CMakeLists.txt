cmake_minimum_required(VERSION 3.12)

######################################################

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
IF (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
ELSE ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
ENDIF ()

######################################################

project(01_basic)

add_definitions(-DPATH_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../sources")

# openvdb
list(APPEND CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/openvdblib/lib/cmake/OpenVDB
)
message(CMAKE_MODULE_PATH)
find_package(OpenVDB REQUIRED)

# opengl
find_package(OpenGL REQUIRED)

# glfw
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/glfwlib)
find_package(glfw3 REQUIRED)

# glad
set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/glad4/include)
get_filename_component(GLAD_INCLUDE_DIR ${GLAD_INCLUDE_DIR} ABSOLUTE)

# glm
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/glmlib/include)

######################################################

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/../external)
add_executable(${PROJECT_NAME}
    main.cpp
    ${EXTERNAL}/glad4/include/glad/glad.h
    ${EXTERNAL}/glad4/src/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/camera.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/glfwFunc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/openVDB.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/shader.h
)

target_link_libraries(${PROJECT_NAME}
    OpenVDB::openvdb
    ${OPENGL_LIBRARY}
    glfw
)