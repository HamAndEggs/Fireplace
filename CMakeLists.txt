cmake_minimum_required(VERSION 3.18.0)
set(THIS_PROJECT fireplace)
project(${THIS_PROJECT} VERSION 0.1.0)


if( TARGET_PLATFORM STREQUAL "GTK4" )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED gtk4)
endif()


include_directories(
    /usr/include/freetype2
    /usr/include/libdrm
    /usr/include/gstreamer-1.0
    ./EdgeUI/include
    ./source
    ${GTK4_INCLUDE_DIRS}
)

add_subdirectory(./EdgeUI)

add_executable(${THIS_PROJECT}
    ./source/DisplayAirQuality.cpp
    ./source/DisplayBitcoinPrice.cpp
    ./source/DisplayClock.cpp
    ./source/DisplaySystemStatus.cpp
    ./source/DisplayWeather.cpp
    ./source/DisplaySolaX.cpp
    ./source/Temperature.cpp
    ./source/i2c_device.cpp
    ./source/main.cpp
    ./source/MQTTData.cpp
    ./source/sgp30.cpp
    ./source/TinyTools.cpp
    ./source/TinyWeather.cpp
    ./source/DisplayTideData.cpp
    ./source/FileDownload.cpp
    ./source/VideoToTexture.cpp
    ./source/DaytimeDisplay.cpp
)

set_property(TARGET ${THIS_PROJECT} PROPERTY CXX_STANDARD 17)
set(COMMON_LIBS stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl avcodec avfilter avformat avdevice avutil swresample)

if( TARGET_PLATFORM STREQUAL "GTK4" )
    target_link_libraries(${THIS_PROJECT} EdgeUI.GTK4 ${COMMON_LIBS} ${GTK4_LINK_LIBRARIES})
elseif( TARGET_PLATFORM STREQUAL "DRM" )
    target_link_libraries(${THIS_PROJECT} EdgeUI.DRM ${COMMON_LIBS} gbm drm)
elseif( TARGET_PLATFORM STREQUAL "X11" )
    target_link_libraries(${THIS_PROJECT} EdgeUI.X11 ${COMMON_LIBS} X11 GL)
else()
    message(FATAL_ERROR "TARGET_PLATFORM not defined")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

