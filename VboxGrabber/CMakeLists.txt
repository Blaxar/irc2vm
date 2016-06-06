cmake_minimum_required(VERSION 2.6)
 
project(VboxGrabber)
 
set(CMAKE_DEBUG_POSTFIX "_d")
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
"${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(FFmpeg REQUIRED COMPONENTS avutil avcodec avformat
swscale OPTIONAL_COMPONENTS swresample)

message(${FFMPEG_LIBRARIES})

set(DEFAULT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(DEFAULT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(DEFAULT_LINK_DIR ${CMAKE_SOURCE_DIR}/lib)
set(DEFAULT_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)

set(VBOX_INSTALL_DIR /usr/lib/virtualbox)
set(VBOX_LIBS ${VBOX_INSTALL_DIR}/VBoxXPCOM.so)
set(VBOX_INCLUDE_DIR ${VBOX_INSTALL_DIR}/sdk/bindings/xpcom/include)


#Define all the include directories
include_directories(
    ${DEFAULT_INCLUDE_DIR}
	${FFMPEG_INCLUDE_DIRS}
	${VBOX_INCLUDE_DIR}
	${VBOX_INCLUDE_DIR}/nsprpub
    ${VBOX_INCLUDE_DIR}/string
    ${VBOX_INCLUDE_DIR}/xpcom
    ${VBOX_INCLUDE_DIR}/ipcd
)

add_definitions(-std=c++14 -O3 -ldl -lpthread -fshort-wchar -DVBOX_WITH_XPCOM_NAMESPACE_CLEANUP -DRT_OS_LINUX)

link_directories(${DEFAULT_LINK_DIR})
add_subdirectory(src)
