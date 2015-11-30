cmake_minimum_required (VERSION 2.6)
cmake_policy(VERSION 3.0)

# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (JStd)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

include(CMake/cotire.cmake)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set compiler flags and options. 
# Here it is setting the Visual Studio warning level to 4
if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif (CMAKE_COMPILER_IS_GNUCXX )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
endif(MSVC)

add_definitions(-D_AFXDLL -DWINVER=0x600 -D_WIN32_WINNT=0x600)
set(CMAKE_MFC_FLAG 2)

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags: " ${CMAKE_CXX_FLAGS})

# Sub-directories where more CMakeLists.txt exist
set(JSTD_SOURCES_CPP
	"pchheader.cpp"
	"JStd.cpp"
	)
set(JSTD_SOURCES_H
	"JStd.h"
	)

if(WIN32)
    set(JSTD_SOURCES_CPP
        ${JSTD_SOURCES_CPP}
        "CmdLine.cpp"
        "DirIterator.cpp"
        "JCoro.cpp"
        "Sha1.cpp"
        "Threading.cpp"
        "JWnd.cpp"
        "JGraphics.cpp"
        "JOpenGl.cpp"
        )
    set(JSTD_SOURCES_H
        ${JSTD_SOURCES_H}
        "CmdLine.h"
        "DirIterator.h"
        "JCoro.h"
        "Sha1.h"
        "Threading.h"
        "JWnd.h"
        "JGraphics.h"
        "JOpenGl.h"
        )
endif()

source_group("include" FILES ${JSTD_SOURCES_H})
source_group("src"     FILES ${JSTD_SOURCES_CPP})

add_definitions(-DJ_CREATING_LIB)
add_library(JStd STATIC ${JSTD_SOURCES_CPP}
                        ${JSTD_SOURCES_H})
if(MSVC)
    set_target_properties(JStd PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pchheader.h")
    cotire(JStd)
    target_link_libraries(JStd glu32 opengl32 Comctl32)
endif()
