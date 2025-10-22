cmake_minimum_required(VERSION 3.16)
project(nbp_gold C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# ==== EXECUTABLE SOURCES ====
add_executable(nbp_gold
        src/main.c
        src/http_client.c      # jeśli na razie nie łączysz z libcurl, możesz tymczasowo usunąć tę linię
        src/json_utils.c
        src/gold_quote.c
        src/http_server.c
)

# ==== VENDORED cJSON ====
add_library(cjson_local STATIC
        third_party/cjson/cJSON.c
)
target_include_directories(cjson_local PUBLIC
        ${CMAKE_SOURCE_DIR}/src/third_party/cjson
)

# ==== VENDORED microhttpd (stub) ====
add_library(microhttpd_local STATIC
        third_party/microhttpd/microhttpd.c
)
target_include_directories(microhttpd_local PUBLIC
        ${CMAKE_SOURCE_DIR}/src/third_party/microhttpd
)

# ==== (opcjonalnie) libcurl – jeśli masz brew curl i http_client.c tego wymaga ====
# Jeżeli nie masz brew, albo chcesz najpierw zbudować bez HTTP, zakomentuj cały blok CURL oraz usuń http_client.c z add_executable.
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CURL libcurl)
    if(CURL_FOUND)
        target_include_directories(nbp_gold PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(nbp_gold ${CURL_LIBRARIES})
    endif()
endif()

# ==== LINKUJEMY NASZE BIBLIOTEKI ====
target_link_libraries(nbp_gold
        cjson_local
        microhttpd_local
)

target_compile_options(nbp_gold PRIVATE -Wall -Wextra -Wpedantic)
