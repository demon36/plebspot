cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set( CMAKE_SUPPORT_WINDOWS_EXPORT_ALL_SYMBOLS 1 )
project (plebspot)

file(GLOB DEP_SRC_FILES thirdparty/captcha/src/*.c)
add_library(captcha STATIC ${DEP_SRC_FILES})
target_include_directories(captcha PUBLIC thirdparty/captcha/include)

file(GLOB DEP_SRC_FILES thirdparty/fmt/src/*.cc)
add_library(fmt STATIC ${DEP_SRC_FILES})
target_include_directories(fmt PUBLIC thirdparty/fmt/include)

file(GLOB DEP_SRC_FILES thirdparty/cpp-base64/base64.cpp)
add_library(cpp-base64 STATIC ${DEP_SRC_FILES})
target_include_directories(cpp-base64 PUBLIC thirdparty/cpp-base64)

file(GLOB DEP_SRC_FILES thirdparty/hoedown/src/*.c)
add_library(hoedown STATIC ${DEP_SRC_FILES})
target_include_directories(hoedown PUBLIC thirdparty/hoedown/src)

file(GLOB DEP_SRC_FILES thirdparty/pugixml/src/*.cpp)
add_library(pugixml STATIC ${DEP_SRC_FILES})
target_include_directories(pugixml PUBLIC thirdparty/pugixml/src)


file(GLOB SRC_FILES src/*.cpp)
add_executable(plebspot ${SRC_FILES})
target_include_directories(plebspot PRIVATE thirdparty/Mustache thirdparty/plusaes/include thirdparty/cpp-httplib)
target_link_libraries(plebspot fmt captcha cpp-base64 hoedown pugixml)

if(WIN32)
	target_link_libraries(plebspot ws2_32)
endif()