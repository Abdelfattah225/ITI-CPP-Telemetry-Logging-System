# Create logging library
add_library(logging 
    logging/ConsoleSinkImpl.cpp
    logging/FileSinkImpl.cpp
    logging/LogManager.cpp
    )

# Set include directory for this library
target_include_directories(logging PUBLIC ${PROJECT_SOURCE_DIR}/inc/logging)

