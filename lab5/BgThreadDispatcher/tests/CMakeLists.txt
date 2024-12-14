add_executable(BgThreadDispatcherTests
        ../BgThreadDispatcher.h
        BgThreadDispatcher.test.cpp
)

target_link_libraries(BgThreadDispatcherTests PUBLIC catch2)
add_test(NAME BgThreadDispatcherTests COMMAND BgThreadDispatcherTests)
