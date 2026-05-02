/*
 * File: boot_types.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares shared bootloader enums, status codes, and protocol data types.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_TYPES_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_TYPES_H

#include <stdint.h>

typedef enum
{
    BOOT_REQ_NONE = 0,
    BOOT_REQ_APP,
    BOOT_REQ_PROGRAMMER
} boot_request_t;

typedef enum
{
    BOOT_STATUS_OK = 0,
    BOOT_STATUS_ERROR,
    BOOT_STATUS_TIMEOUT,
    BOOT_STATUS_INVALID_ARGUMENT,
    BOOT_STATUS_INVALID_IMAGE,
    BOOT_STATUS_IO_ERROR,
    BOOT_STATUS_CRC_MISMATCH,
    BOOT_STATUS_INVALID_STATE,
    BOOT_STATUS_INVALID_MAGIC,
    BOOT_STATUS_INVALID_HEADER_VERSION,
    BOOT_STATUS_INVALID_HEADER_SIZE,
    BOOT_STATUS_INVALID_IMAGE_TYPE,
    BOOT_STATUS_INVALID_VECTOR_ADDR,
    BOOT_STATUS_INVALID_IMAGE_SIZE,
    BOOT_STATUS_INVALID_STACK_PTR,
    BOOT_STATUS_INVALID_RESET_VECTOR
} boot_status_t;

typedef enum
{
    BOOT_IMAGE_TYPE_BOOTMANAGER = 1,
    BOOT_IMAGE_TYPE_PROGRAMMER,
    BOOT_IMAGE_TYPE_APP
} boot_image_type_t;

typedef enum
{
    UPDATE_IDLE = 0,
    UPDATE_RUNNING,
    UPDATE_OK,
    UPDATE_FAIL
} update_status_t;

typedef struct
{
    uint32_t magic;
    uint32_t boot_request;
    uint32_t update_status;
    uint32_t error_code;
    uint32_t reserved[4];
} boot_shared_t;

typedef struct
{
    uint8_t cmd;
    uint16_t len;
    uint8_t data[256];
    uint8_t crc;
} boot_packet_t;

#endif
