# Define podo paths independently 
set(PODO_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Root directory for podo library")

# Verify podo directory exists
if(NOT EXISTS ${PODO_ROOT_DIR})
    message(FATAL_ERROR "PODO_ROOT_DIR '${PODO_ROOT_DIR}' does not exist. Please set correct path.")
endif()

# Component directories relative to PODO_ROOT_DIR
set(PODO_COMPONENTS
    Core
    HAL
    UI
)

# Add podo library
add_library(podo_app STATIC)

# Collect sources from each component
foreach(component ${PODO_COMPONENTS})
    file(GLOB_RECURSE COMPONENT_SOURCES "${PODO_ROOT_DIR}/${component}/*.c")
    target_sources(podo_app PRIVATE ${COMPONENT_SOURCES})
    target_include_directories(podo_app 
        PUBLIC 
            "${PODO_ROOT_DIR}/${component}"
            "${PODO_ROOT_DIR}/${component}/Inc"  # Add Inc subdirectory if you use it
    )
endforeach()

# Add LVGL dependency and include paths
target_link_libraries(podo_app PUBLIC lvgl)
target_include_directories(podo_app PUBLIC 
    ${CMAKE_SOURCE_DIR}     # Root project directory for lvgl.h
    ${CMAKE_SOURCE_DIR}/lvgl  # LVGL directory
    ${PODO_ROOT_DIR}  # Add root podo directory for direct includes
)

# Export podo_app target
set_target_properties(podo_app PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)