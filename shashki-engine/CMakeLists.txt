set(HEADERS include/shashki-engine/common.hpp
            include/shashki-engine/move-generation.hpp
            include/shashki-engine/evaluation.hpp)

set(SOURCES src/common.cpp
            src/move-generation.cpp
            src/evaluation.cpp)

add_library(shashki-engine ${HEADERS} ${SOURCES})

target_include_directories(shashki-engine PUBLIC include)
