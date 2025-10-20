# Bibliothèque statique (.a)
add_library(LifeTagLibStatic STATIC
    src/version.cpp
)

# Bibliothèque dynamique (.so)
add_library(LifeTagLibShared SHARED
    src/version.cpp
)

# Répertoires include
target_include_directories(LifeTagLibStatic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
target_include_directories(LifeTagLibShared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Pour que la lib .so ait un nom propre sans "libShared"
set_target_properties(LifeTagLibShared PROPERTIES OUTPUT_NAME "LifeTagLib")
set_target_properties(LifeTagLibStatic PROPERTIES OUTPUT_NAME "LifeTagLib")
