add_executable(translator translator.cc)

target_include_directories(translator PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)