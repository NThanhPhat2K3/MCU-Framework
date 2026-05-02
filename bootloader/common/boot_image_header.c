/*
 * File: boot_image_header.c
 * Author: Phat Nguyen
 * Date: 2026-05-02
 * Description: Emits the per-image header blob that is patched after link with size and CRC.
 */

#include "boot_image.h"

#include "boot_config.h"

#if defined(IMAGE_BOOTMANAGER)
#define BOOT_IMAGE_THIS_TYPE BOOT_IMAGE_TYPE_BOOTMANAGER
#define BOOT_IMAGE_THIS_STATE BOOT_IMAGE_STATE_VALID
#define BOOT_IMAGE_THIS_ADDR BOOT_MANAGER_ADDR
#elif defined(IMAGE_PROGRAMMER)
#define BOOT_IMAGE_THIS_TYPE BOOT_IMAGE_TYPE_PROGRAMMER
#define BOOT_IMAGE_THIS_STATE BOOT_IMAGE_STATE_VALID
#define BOOT_IMAGE_THIS_ADDR PROGRAMMER_ADDR
#elif defined(IMAGE_APP)
#define BOOT_IMAGE_THIS_TYPE BOOT_IMAGE_TYPE_APP
#define BOOT_IMAGE_THIS_STATE BOOT_IMAGE_STATE_PENDING
#define BOOT_IMAGE_THIS_ADDR APP_ADDR
#else
#error boot_image_header.c requires an IMAGE_* define
#endif

__attribute__((used, section(".image_header"), aligned(4))) const boot_image_header_t
    g_boot_image_header = {
        .magic = BOOT_IMAGE_MAGIC,
        .header_version = BOOT_IMAGE_HEADER_VERSION,
        .header_size = sizeof(boot_image_header_t),
        .image_type = BOOT_IMAGE_THIS_TYPE,
        .image_state = BOOT_IMAGE_THIS_STATE,
        .image_size = 0u,
        .vector_addr = BOOT_IMAGE_THIS_ADDR,
        .crc32 = 0u,
        .version_major = 0u,
        .version_minor = 0u,
        .version_patch = 0u,
        .reserved = {0u, 0u, 0u, 0u, 0u, 0u}};
