find_package(X11 REQUIRED)

if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
else ()
    message(FATAL_ERROR "X11 library not found")
endif ()

add_executable(snowflakes
        X11Window.h
        SnowfallApp.h
        main.cpp)

target_link_libraries(snowflakes ${X11_LIBRARIES})
