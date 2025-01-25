
###########################################################################
#                                                                         #
# Find SDL2 library                                                       #
#                                                                         #
###########################################################################


# SDL2 headers
find_path(SDL2_INCLUDE_DIR ./SDL.h
    HINTS ${INTERNAL_LIBS_PATH}/include/SDL2
)

# SDL2 library
find_library(SDL2_LIBRARY
    NAMES SDL2
    HINTS ${INTERNAL_LIBS_PATH}/lib
)

# SDL2main library
find_library(SDL2MAIN_LIBRARY
    NAMES SDL2main
    HINTS ${INTERNAL_LIBS_PATH}/lib
)

#message(STATUS "SDL2main lib: ${SDL2MAIN_LIBRARY}")

# If SDL2 is found, define the target for linking
if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY AND SDL2MAIN_LIBRARY)
set(SDL2_FOUND TRUE)
message(STATUS " -> SDL2 found")

add_library(SDL2::SDL2 INTERFACE IMPORTED)
set_target_properties(SDL2::SDL2 PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARY}"
)

add_library(SDL2::SDL2main INTERFACE IMPORTED)
set_target_properties(SDL2::SDL2main PROPERTIES
        INTERFACE_LINK_LIBRARIES "${SDL2MAIN_LIBRARY}"
        )
        
        else()
        set(SDL2_FOUND FALSE)
        message(WARNING " -> SDL2 not found!")
endif()



# PREVERJANJE CE JE NAJDU VSAKO STVAR

#if(SDL2_LIBRARY)
#    message(STATUS "NAJDU SDL2 lib: ${SDL2_LIBRARY}")
#endif()
#if(SDL2MAIN_LIBRARY)
#    message(STATUS "NAJDU SDL2main lib: ${SDL2MAIN_LIBRARY}")
#endif()
#if(SDL2_INCLUDE_DIR)
#    message(STATUS "NAJDU INCLUDE DIR: ${SDL2_INCLUDE_DIR}")
#endif()