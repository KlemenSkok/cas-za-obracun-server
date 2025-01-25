
###########################################################################
#                                                                         #
# Find spdlog library                                                       #
#                                                                         #
###########################################################################


set(TMP_SPDLOG_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${INTERNAL_LIBS_PATH}/include")
set(TMP_SPDLOG_LIBRARY ${INTERNAL_LIBS_PATH}/lib)

unset(CMAKE_INCLUDE_PATH CACHE)
# spdlog headers
#find_path(SPDLOG_INCLUDE_DIR
#    NAMES spdlog/spdlog.h spdlog.h spdlog
#    ${TMP_SPDLOG_INCLUDE_DIR}
#    NO_DEFAULT_PATH
#)

# force fixed path
set(SPDLOG_INCLUDE_DIR ${TMP_SPDLOG_INCLUDE_DIR})


# spdlog library
find_library(SPDLOG_LIBRARY
    NAMES spdlog spdlogd
    HINTS ${TMP_SPDLOG_LIBRARY}
)
set(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include) # Restore if needed


#message(STATUS "spdlog lib: ${SPDLOG_LIBRARY}")
#message(STATUS "spdlog include: ${SPDLOG_INCLUDE_DIR}")

# If spdlog is found, define the target for linking
if(SPDLOG_INCLUDE_DIR AND SPDLOG_LIBRARY)
    set(SPDLOG_FOUND TRUE)
    message(STATUS " -> spdlog found")

    add_library(spdlog::spdlog INTERFACE IMPORTED)
    set_target_properties(spdlog::spdlog PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SPDLOG_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${SPDLOG_LIBRARY}"
    )
else()
    set(SPDLOG_FOUND FALSE)
    message(WARNING " -> spdlog not found!")
endif()



# PREVERJANJE CE JE NAJDU VSAKO STVAR

#if(SPDLOG_LIBRARY)
#    message(STATUS "NAJDU spdlog lib: ${SPDLOG_LIBRARY}")
#endif()
#if(SPDLOG_INCLUDE_DIR)
#    message(STATUS "NAJDU INCLUDE DIR: ${SPDLOG_INCLUDE_DIR}")
#endif()