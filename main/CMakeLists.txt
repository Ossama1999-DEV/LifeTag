# Exécutable principal
add_executable(LifeTagApp main.cpp)

# Link avec la lib (statique ou partagée)
target_link_libraries(LifeTagApp PRIVATE LifeTagLibShared)

# Très important : dire à CMake d’hériter aussi des includes publics de la lib
target_include_directories(LifeTagApp PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/inc
)
