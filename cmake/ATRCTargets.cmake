# Create imported target
add_library(ATRC::ATRC SHARED IMPORTED)
set_property(TARGET ATRC::ATRC PROPERTY IMPORTED_GLOBAL TRUE)

set_target_properties(ATRC::ATRC PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release"
    INTERFACE_INCLUDE_DIRECTORIES "${ATRC_INCLUDE_DIR}"
    
    IMPORTED_LOCATION_RELEASE "${ATRC_LIB_RELEASE}"
    IMPORTED_LOCATION_DEBUG "${ATRC_LIB_DEBUG}"
)

# Set IMPORTED_LOCATION only if not multi-config (fallback)
if(NOT CMAKE_CONFIGURATION_TYPES)
    set_target_properties(ATRC::ATRC PROPERTIES
        IMPORTED_LOCATION "${ATRC_LIB_RELEASE}"
    )
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(ATRC::ATRC PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${ATRC_LIB_RELEASE}"
        IMPORTED_IMPLIB_DEBUG "${ATRC_LIB_DEBUG}"
        IMPORTED_LOCATION_RELEASE "${ATRC_DLL_RELEASE}"
        IMPORTED_LOCATION_DEBUG "${ATRC_DLL_DEBUG}"
    )
endif()


# Link interface (optional unless using CMake <3.0 or doing custom things)
set_target_properties(ATRC::ATRC PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C;CXX"
)
