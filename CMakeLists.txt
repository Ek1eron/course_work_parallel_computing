cmake_minimum_required(VERSION 3.20)
project(coursework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ====== Targets ======
add_executable(coursework
        src/main.cpp
        src/index/InvertedIndex.cpp
        src/concurrent/ConcurrentMap.cpp
        src/server/SearchServer.cpp
        src/threading/ThreadPool.cpp
        src/utils/FileLoader.cpp
        src/utils/Tokenizer.cpp
)

add_executable(client
        src/client/Client.cpp
)

# ====== Windows libs ======
if (WIN32)
    target_link_libraries(coursework PRIVATE ws2_32)
    target_link_libraries(client PRIVATE ws2_32)
endif()

# ====== Option: try to build fully static ======
option(STATIC_RUNTIME "Link MinGW runtime statically (no DLLs needed)" OFF)

if (STATIC_RUNTIME)
    message(STATUS "STATIC_RUNTIME=ON: trying fully static build")

    target_link_options(coursework PRIVATE -static -static-libstdc++ -static-libgcc)
    target_link_options(client PRIVATE    -static -static-libstdc++ -static-libgcc)

    # статично підтягуємо winpthread універсальним способом
    target_link_libraries(coursework PRIVATE -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic)
    target_link_libraries(client PRIVATE    -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic)

else()
    message(STATUS "STATIC_RUNTIME=OFF: build with DLLs and copy them next to exe")

    # Після збірки копіюємо DLL з toolchain/bin у папку з .exe
    if (MINGW)
        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        set(RUNTIME_DLLS
                libstdc++-6.dll
                libgcc_s_seh-1.dll
                libwinpthread-1.dll
        )

        foreach(dll ${RUNTIME_DLLS})
            add_custom_command(TARGET coursework POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_DIR}/${dll}"
                    "$<TARGET_FILE_DIR:coursework>/${dll}"
                    COMMENT "Copying ${dll} for coursework.exe"
            )
            add_custom_command(TARGET client POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_DIR}/${dll}"
                    "$<TARGET_FILE_DIR:client>/${dll}"
                    COMMENT "Copying ${dll} for client.exe"
            )
        endforeach()
    endif()
endif()
