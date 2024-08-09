###### CMakeLists supposed to download external dependencies

##### Remove warnings and keep only optimization flags

set(CMAKE_CXX_FLAGS_COPY "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-w")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
    # Build shared libraries to reduce linking time
    set(BUILD_SHARED_LIBS ON)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
    # Build static libraries to improve performance
    set(BUILD_SHARED_LIBS OFF)
endif()

#### Set assimp build flags to skip unnecessary features for faster compilation
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(DASSIMP_BUILD_LOG OFF)

#### FetchContent
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${VENDOR_DIR})

#### GLAD
FetchContent_Declare(
    GLAD
    URL https://gitfront.io/r/Moztanku/8s4L1GHF3Tsc/Resources/raw/glad.zip
)
FetchContent_MakeAvailable(GLAD)

add_library(GLAD STATIC ${glad_SOURCE_DIR}/src/gl.c)
target_include_directories(GLAD PUBLIC ${glad_SOURCE_DIR}/include)

#### glm
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
FetchContent_MakeAvailable(glm)

#### glfw3
FetchContent_Declare(
    GLFW
    URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)
FetchContent_MakeAvailable(GLFW)

#### Assimp
FetchContent_Declare(
    assimp
    URL https://github.com/assimp/assimp/archive/refs/tags/v5.4.2.zip
)
FetchContent_MakeAvailable(assimp)

#### JacekLib
FetchContent_Declare(
    JacekLib
    URL http://github.com/Moztanku/JacekLib/archive/main.zip
)
FetchContent_MakeAvailable(JacekLib)

#### STB_image
# vendor/stb
set(STB_DIR ${VENDOR_DIR}/stb)
# Link to STB_image header file from STB repository
set(STB_IMG_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")

if(NOT EXISTS ${STB_DIR}/inc/stb_image.h)
    file(DOWNLOAD ${STB_IMG_URL} ${STB_DIR}/inc/stb_image.h SHOW_PROGRESS)
endif()

add_library(STB_image INTERFACE)
target_include_directories(STB_image INTERFACE ${STB_DIR}/inc)

##### Restore original flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_COPY}")
