cmake_minimum_required(VERSION 3.20)
project(orbit_engine LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

set(ORBIT_ENGINE_SOURCES
    ExceptionHandler.cpp
    Item.cpp
    Scoring.cpp
    TextUtils.cpp
    JsonItemParser.cpp
    InvertedIndex.cpp
    MatchingEngine.cpp
)

add_executable(orbit-engine main.cpp ${ORBIT_ENGINE_SOURCES})
add_executable(orbit-engine-jsonl json_lines_main.cpp ${ORBIT_ENGINE_SOURCES})
target_compile_features(orbit-engine PRIVATE cxx_std_17)
target_link_libraries(orbit-engine PRIVATE nlohmann_json::nlohmann_json)
target_compile_features(orbit-engine-jsonl PRIVATE cxx_std_17)
target_link_libraries(orbit-engine-jsonl PRIVATE nlohmann_json::nlohmann_json)
