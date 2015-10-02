set(FATFS_DIR ${third_party_dir}/FATFS)

file(GLOB src ${FATFS_DIR}/src/*.c)
add_library(FATFS
    ${src}
    ${FATFS_DIR}/src/drivers/sd_diskio.c
)
target_include_directories(FATFS PRIVATE
    ${FATFS_DIR}/src
    ${FATFS_DIR}/src/drivers
)
