# CMAKE required minimum version
cmake_minimum_required(VERSION 3.6)

# Project (binary) name
project(SolarSystem)

#Set bin output
set(EXECUTABLE_OUTPUT_PATH "Debug")

########################################################################################
#    DETECT OSX
########################################################################################
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Set a global flag for OSX
    set(MACOSX TRUE)
ENDIF()
########################################################################################

# Detect Operating-System
IF (WIN32)
    ####################################################################################
    #    WINDOWS (copy SDL2.dll and others to your binary folder ./bin/Debug/)
    ####################################################################################

    # SDL2
    set(SDL2_INCLUDE_DIR D:/OpenGL/SDL2-2.0.9/x86_64-w64-mingw32/include)
    set(SDL2_LIBRARY_DIR D:/OpenGL/SDL2-2.0.9/x86_64-w64-mingw32/lib)

    # GLEW
    set(GLEW_INCLUDE_DIR D:/OpenGL/glew-2.1.0/include)
    set(GLEW_LIBRARY_DIR D:/OpenGL/glew-2.1.0/lib/Release/x64)

    # Freeimage
    set(FREEIMAGE_INCLUDE_DIR D:/OpenGL/FreeImage-3.18.0/Dist/x64)
    set(FREEIMAGE_LIBRARY_DIR D:/OpenGL/FreeImage-3.18.0/Dist/x64)

    # FreeimagePlus
    set(FREEIMAGEPLUS_INCLUDE_DIR D:/OpenGL/FreeImage-3.18.0/Wrapper/FreeImagePlus/dist/x64)
    set(FREEIMAGEPLUS_LIBRARY_DIR D:/OpenGL/FreeImage-3.18.0/Wrapper/FreeImagePlus/dist/x64)

    # GLM
    set(GLM_INCLUDE_DIR D:/OpenGL/glm-0.9.9.4/include)

    # include & link lib-dirs
    include_directories(${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${FREEIMAGE_INCLUDE_DIR} ${FREEIMAGEPLUS_INCLUDE_DIR})
    link_directories(${SDL2_LIBRARY_DIR} ${GLEW_LIBRARY_DIR} ${FREEIMAGE_LIBRARY_DIR} ${FREEIMAGEPLUS_LIBRARY_DIR})

    ####################################################################################

    # Additional Compiler Flags
    set(COMPILER_FLAGS "-lmingw32 -static-libstdc++ -static-libgcc")

    # Libraries to be linked
    set(LINK_LIBRARIES SDL2main SDL2 opengl32 glew32 freeimage)

ELSEIF(MACOSX)
    ####################################################################################
    #    OSX
    ####################################################################################

    # Add custom sdl2-cmake files to cmake module path
    #   (install sdl2-framework[s] & copy sdl2_cmake_scripts to your project-root (/bin/cmake/sdl2/FindSDL2***.cmake))
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/cmake/sdl2)

    # run FindSDL2.cmake helper script
    find_package(SDL2 REQUIRED)

    #  (install via homebrew (brew install glew glm))
    find_package(GLEW REQUIRED)

    # include-dirs
    set(APPLE_INCLUDE_DIR /usr/local/include)

    # lib-dirs
    set(APPLE_LIB_DIR /usr/local/lib)

    # include dirs
    include_directories(${APPLE_INCLUDE_DIR})

    link_directories(${APPLE_LIB_DIR})

    ####################################################################################

    # Libraries to be linked
    set(LINK_LIBRARIES ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${APPLE_LIB_DIR} freeimage "-framework OpenGL")

ELSE()
    ####################################################################################
    #    UNIX
    ####################################################################################

    # Libraries to be linked
    set(LINK_LIBRARIES SDL2 GL GLEW freeimage)

ENDIF()

########################################################################################
#    4All
########################################################################################

# Set C++ version via compiler flag
set(GLOBAL_COMPILER_FLAGS "-std=c++11")

# Set All Compiler Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_COMPILER_FLAGS} ${COMPILER_FLAGS}")

# Set source files
set(SOURCE_FILES SolarSystem.cpp SolarSystem.h Camera.cpp Camera.h Shader.cpp Shader.h Texture.cpp Texture.h Skybox.cpp Skybox.h Moon.cpp Moon.h Sphere.cpp Sphere.h)

# Set (add) executable name
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link (runtime) libraries
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})
