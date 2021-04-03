# Header-only
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO cpsusie/cjm-numerics
    REF ExpRel
    SHA512 1
    HEAD_REF main
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)
vcpkg_install_cmake()


