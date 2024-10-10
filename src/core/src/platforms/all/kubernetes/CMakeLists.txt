# Make an automatic library - will be static or dynamic based on user setting
add_library(wolf_kubernetes)
add_library(wolf::kubernetes ALIAS wolf_kubernetes)

target_include_directories(wolf_kubernetes PUBLIC . PRIVATE ../../../)
target_sources(wolf_kubernetes
        PRIVATE
        kubernetes/kubernetes.cpp)

find_package(CURL) # REQUIRED UnixSockets removed because it doesn't work in Github Actions
target_link_libraries_system(wolf_kubernetes
        PRIVATE
        CURL::libcurl
        wolf::helpers)

FetchContent_Declare(
        yaml
        GIT_REPOSITORY https://github.com/yaml/libyaml.git
        GIT_TAG 0.2.5
        OVERRIDE_FIND_PACKAGE)

FetchContent_Declare(
        kubernetes
        GIT_REPOSITORY https://github.com/kubernetes-client/c.git
        GIT_TAG v0.10.0
        SOURCE_SUBDIR "kubernetes")
FetchContent_MakeAvailable(kubernetes)
target_link_libraries_system(wolf_kubernetes PRIVATE kubernetes)

# All users of this library will need at least C++17
target_compile_features(wolf_kubernetes PUBLIC cxx_std_17)