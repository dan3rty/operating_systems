add_executable(MemoryManagerTests
        ../MemoryManager.h
        MemoryManager.tests.cpp
)

target_link_libraries(MemoryManagerTests PUBLIC catch2)
add_test(NAME MemoryManagerTests COMMAND MemoryManagerTests)
