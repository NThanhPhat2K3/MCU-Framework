#include "boot_proto.h"

#include "port_uart.h"

#include <string.h>

static uint8_t boot_proto_crc(uint8_t cmd, uint16_t len, const uint8_t *data)
{
    uint8_t crc = cmd ^ (uint8_t)len ^ (uint8_t)(len >> 8);
    uint16_t i;

    for (i = 0; i < len; ++i)
    {
        crc ^= data[i];
    }

    return crc;
}

boot_status_t boot_proto_recv(boot_packet_t *pkt)
{
    uint8_t header[3];

    if (port_uart_read(header, 3u, PORT_UART_TIMEOUT_FOREVER) != 0)
    {
        return BOOT_STATUS_IO_ERROR;
    }

    pkt->cmd = header[0];
    pkt->len = (uint16_t)header[1] | ((uint16_t)header[2] << 8);

    if (pkt->len > sizeof(pkt->data))
    {
        return BOOT_STATUS_INVALID_ARGUMENT;
    }

    if ((pkt->len > 0u) && (port_uart_read(pkt->data, pkt->len, PORT_UART_TIMEOUT_FOREVER) != 0))
    {
        return BOOT_STATUS_IO_ERROR;
    }

    if (port_uart_read(&pkt->crc, 1u, PORT_UART_TIMEOUT_FOREVER) != 0)
    {
        return BOOT_STATUS_IO_ERROR;
    }

    return (pkt->crc == boot_proto_crc(pkt->cmd, pkt->len, pkt->data))
               ? BOOT_STATUS_OK
               : BOOT_STATUS_ERROR;
}

boot_status_t boot_proto_send_text(const char *text)
{
    return (port_uart_write((const uint8_t *)text, (uint32_t)strlen(text)) == 0)
               ? BOOT_STATUS_OK
               : BOOT_STATUS_IO_ERROR;
}
