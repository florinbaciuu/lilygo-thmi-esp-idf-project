

idf_component_register(
    SRCS "src/ResourceMonitor.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES driver freertos esp_timer
)

# Asta adaug-o separat:
set(KCONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Kconfig")

# Activează automat definirea opțiunilor Kconfig
#target_compile_definitions(${COMPONENT_LIB} PRIVATE "-DRESOURCE_MONITOR_CONFIG_FROM_KCONFIG")

