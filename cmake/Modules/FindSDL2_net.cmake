
###########################################################################
#                                                                         #
# Find SDL2_net library                                                   #
#                                                                         #
###########################################################################


# SDL2_net headers
find_path(SDL2_NET_INCLUDE_DIR SDL_net.h
    HINTS ${INTERNAL_LIBS_PATH}/include/SDL2
)

# SDL2_net library
find_library(SDL2_NET_LIBRARY
    NAMES SDL2_net
    HINTS ${INTERNAL_LIBS_PATH}/lib
)

# If SDL2_ttf is found, define the target for linking
if(SDL2_NET_INCLUDE_DIR AND SDL2_NET_LIBRARY)
set(SDL2_NET_FOUND TRUE)
message(STATUS " -> SDL2_net found")

add_library(SDL2_NET::SDL2_NET INTERFACE IMPORTED)
set_target_properties(SDL2_NET::SDL2_NET PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES "${SDL2_NET_INCLUDE_DIR}"
INTERFACE_LINK_LIBRARIES "${SDL2_NET_LIBRARY}"
)
else()
set(SDL2_NET_FOUND FALSE)
message(WARNING " -> SDL2_net not found!")
endif()