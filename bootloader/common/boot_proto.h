/*
 * File: boot_proto.h
 * Author: Phat Nguyen
 * Date: 2026-04-26
 * Description: Declares the UART packet protocol interface used by the programmer image.
 */

#ifndef PORTABLE_BOOT_EXAMPLE_BOOT_PROTO_H
#define PORTABLE_BOOT_EXAMPLE_BOOT_PROTO_H

#include "boot_types.h"

boot_status_t boot_proto_recv(boot_packet_t *pkt);
boot_status_t boot_proto_send_text(const char *text);

#endif
