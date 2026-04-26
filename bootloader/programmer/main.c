#include "boot_config.h"
#include "boot_jump.h"
#include "boot_proto.h"
#include "boot_shared.h"
#include "boot_types.h"
#include "port_flash.h"
#include "port_system.h"
#include "port_uart.h"

#include <stdint.h>

static void programmer_send_status(const char *text)
{
    (void)boot_proto_send_text(text);
}

static boot_status_t programmer_handle_erase(boot_shared_t *shared)
{
    boot_status_t status = port_flash_erase_app();

    if (status == BOOT_STATUS_OK)
    {
        programmer_send_status("OK ERASE\r\n");
    }
    else
    {
        shared->error_code = 1u;
        programmer_send_status("ERR ERASE\r\n");
    }

    return status;
}

static boot_status_t programmer_handle_write(const boot_packet_t *pkt)
{
    uint32_t addr;
    uint32_t data_len;

    if (pkt->len < 4u)
    {
        programmer_send_status("ERR WLEN\r\n");
        return BOOT_STATUS_INVALID_ARGUMENT;
    }

    addr = ((uint32_t)pkt->data[0]) |
           ((uint32_t)pkt->data[1] << 8) |
           ((uint32_t)pkt->data[2] << 16) |
           ((uint32_t)pkt->data[3] << 24);

    data_len = pkt->len - 4u;

    if ((addr < APP_ADDR) || ((addr + data_len) > FLASH_END_ADDR))
    {
        programmer_send_status("ERR ADDR\r\n");
        return BOOT_STATUS_INVALID_ARGUMENT;
    }

    if (port_flash_write(addr, &pkt->data[4], data_len) == BOOT_STATUS_OK)
    {
        programmer_send_status("OK WRITE\r\n");
        return BOOT_STATUS_OK;
    }

    programmer_send_status("ERR WRITE\r\n");
    return BOOT_STATUS_IO_ERROR;
}

static boot_status_t programmer_handle_jump(boot_shared_t *shared)
{
    if (boot_image_check(APP_ADDR) == BOOT_STATUS_OK)
    {
        shared->update_status = UPDATE_OK;
        shared->boot_request = BOOT_REQ_NONE;
        boot_jump_to_image(APP_ADDR);
    }

    shared->update_status = UPDATE_FAIL;
    programmer_send_status("ERR NOAPP\r\n");
    return BOOT_STATUS_INVALID_IMAGE;
}

static void programmer_process_packet(const boot_packet_t *pkt, boot_shared_t *shared)
{
    switch (pkt->cmd)
    {
        case 'I':
            programmer_send_status("OK INFO\r\n");
            break;

        case 'E':
            (void)programmer_handle_erase(shared);
            break;

        case 'W':
            (void)programmer_handle_write(pkt);
            break;

        case 'J':
            (void)programmer_handle_jump(shared);
            break;

        default:
            programmer_send_status("ERR CMD\r\n");
            break;
    }
}

int main(void)
{
    boot_packet_t pkt;
    boot_shared_t *shared;

    port_system_init();
    port_uart_init();
    boot_shared_init();

    shared = boot_shared_get();
    shared->update_status = UPDATE_RUNNING;

    programmer_send_status("PROGRAMMER\r\n");

    for (;;)
    {
        if (boot_proto_recv(&pkt) != BOOT_STATUS_OK)
        {
            programmer_send_status("ERR PKT\r\n");
            continue;
        }
        programmer_process_packet(&pkt, shared);
    }
}
