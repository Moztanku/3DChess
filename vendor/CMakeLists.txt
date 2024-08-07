###### CMakeLists supposed to download external dependencies

#### GLAD

# vendor/GLAD directory
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GLAD)
# Link to generated GLAD archive, if not available generate through command below.
set(GLAD_URL "https://gitfront.io/r/Moztanku/8s4L1GHF3Tsc/Resources/raw/glad.zip")

if (NOT EXISTS ${GLAD_DIR})
    # Download archive and save status
    file(DOWNLOAD ${GLAD_URL} ${GLAD_DIR}/glad.zip SHOW_PROGRESS
        STATUS status)

    list(GET status 0 error_code)

    # Check if download was successful, if not remove the directory and warn the user
    if (NOT ${error_code} EQUAL 0)
        file(REMOVE_RECURSE ${GLAD_DIR})
        message(FATAL_ERROR "Failed to download GLAD, generate it through 'https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon&options=LOADER' and place it in ${GLAD_DIR}")
    endif()

    # Extract the downloaded archive
    file(ARCHIVE_EXTRACT
        INPUT ${GLAD_DIR}/glad.zip
        DESTINATION ${GLAD_DIR})

    file(REMOVE ${GLAD_DIR}/glad.zip)
endif()

add_library(GLAD STATIC ${GLAD_DIR}/src/gl.c)
target_include_directories(GLAD PUBLIC ${GLAD_DIR}/include)

#### glm

# vendor/glm
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm)
# Link to GLM 1.0.1 release
set(GLM_URL "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip")

if(NOT EXISTS ${GLM_DIR})
    # Download archive and save status
    file(DOWNLOAD ${GLM_URL} ${VENDOR_DIR}/glm.zip SHOW_PROGRESS
        STATUS status)

    list(GET status 0 error_code)

    # Check if download was successful, if not remove the directory and warn the user
    if(NOT ${error_code} EQUAL 0)
        file(REMOVE ${VENDOR_DIR}/glm.zip)
        message(FATAL_ERROR "Failed to download GLM, download it from 'https://github.com/g-truc/glm' and place it in ${GLM_DIR}")
    endif()

    # Extract the downloaded archive
    file(ARCHIVE_EXTRACT
        INPUT ${VENDOR_DIR}/glm.zip
        DESTINATION ${VENDOR_DIR})

    
    file(RENAME ${VENDOR_DIR}/glm-1.0.1 ${GLM_DIR})
    file(REMOVE ${VENDOR_DIR}/glm.zip)
endif()

add_subdirectory(${GLM_DIR})

#### glfw3

# vendor/glfw
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
# Link to GLFW 3.4 release
set(GLFW_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip")

if(NOT EXISTS ${GLFW_DIR})
    # Download archive and save status
    file(DOWNLOAD ${GLFW_URL} ${VENDOR_DIR}/glfw.zip SHOW_PROGRESS
        STATUS status)

    list(GET status 0 error_code)

    # Check if download was successful, if not remove the directory and warn the user
    if(NOT ${error_code} EQUAL 0)
        file(REMOVE ${VENDOR_DIR}/glfw.zip)
        message(FATAL_ERROR "Failed to download GLFW, download it from 'https://github.com/glfw/glfw' and place it in ${GLFW_DIR}")
    endif()

    # Extract the downloaded archive
    file(ARCHIVE_EXTRACT
        INPUT ${VENDOR_DIR}/glfw.zip
        DESTINATION ${VENDOR_DIR})

    file(RENAME ${VENDOR_DIR}/glfw-3.4 ${GLFW_DIR})
    file(REMOVE ${VENDOR_DIR}/glfw.zip)
endif()

add_subdirectory(${GLFW_DIR})

#### STB_image

# vendor/stb
set(STB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stb)
# Link to STB_image header file from STB repository
set(STB_IMG_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")

if(NOT EXISTS ${STB_DIR}/inc/stb_image.h)
    file(DOWNLOAD ${STB_IMG_URL} ${STB_DIR}/inc/stb_image.h SHOW_PROGRESS)
endif()

add_library(STB_image INTERFACE)
target_include_directories(STB_image INTERFACE ${STB_DIR}/inc)

## JacekLib

# vendor/JacekLib
set(JacekLib_DIR ${CMAKE_CURRENT_SOURCE_DIR}/JacekLib)
# Link to my JacekLib repository
set(JacekLib_URL "http://github.com/Moztanku/JacekLib/archive/main.zip")

# Download and extract
if (NOT EXISTS ${JacekLib_DIR})
    file(DOWNLOAD ${JacekLib_URL} ${VENDOR_DIR}/JacekLib.zip SHOW_PROGRESS)
    file(ARCHIVE_EXTRACT
        INPUT ${VENDOR_DIR}/JacekLib.zip
        DESTINATION ${VENDOR_DIR})

    file(RENAME ${VENDOR_DIR}/JacekLib-main ${JacekLib_DIR})
    file(REMOVE ${VENDOR_DIR}/JacekLib.zip)
endif()

add_subdirectory(${JacekLib_DIR})

####
