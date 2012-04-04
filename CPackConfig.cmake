include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A local IRC host")
set(CPACK_PACKAGE_VENDOR "The Addams Family")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${LIRCH_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${LIRCH_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${LIRCH_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_TWEAK "${LIRCH_VERSION_TWEAK}")
set(CPACK_PACKAGE_EXECUTABLES "lirch-qt")