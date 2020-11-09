#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "naomi/video.h"
#include "naomi/maple.h"

// Debug console
char *console_base = 0;
unsigned int console_loc = 0;
#define console_printf(...) do { sprintf(console_base + console_loc, __VA_ARGS__); console_loc += strlen(console_base + console_loc); } while(0)

extern uint8_t __z80_fw_1_bin_data[8184];

void display()
{
    // Render a simple test console.
    video_fill_screen(rgb(48, 48, 48));
    video_draw_text(0, 0, rgb(255, 255, 255), console_base);
    video_wait_for_vblank();
    video_display();
}

void main()
{
    #define MENU_SECTION 0xC030000
    volatile unsigned char *menu_section = (volatile unsigned char *)MENU_SECTION;

    *(menu_section+2000)='\0'; // add null
    uint32_t *resp;

    // Set up a crude console
    video_init_simple();
    maple_init();
    console_base = malloc(((640 * 480) / (8 * 8)) + 1);
    memset(console_base, 0, ((640 * 480) / (8 * 8)) + 1);

    // Confirm that this is in fact a new build
    console_printf("RATTBOI WAS HERE\n");
    console_printf("From menu section:\n");
    console_printf("%s\n", menu_section);

    
    // Now, report on the memory test.
    int self_test = maple_request_self_test();
    if(self_test == 1)
    {
        console_printf("MIE reports healthy!\n");
    }
    else
    {
        console_printf("MIE reports bad: %d\n", self_test);
    }
    display();

    /*
    maple_request_reset();
    maple_wait_for_ready();

    uint8_t checksum;

    int update_success = maple_request_update(__z80_fw_1_bin_data, 8184, &checksum);
    if(update_success != 0) {
        console_printf("MIE FW Update failed: %d\n", update_success);
        console_printf("MIE FW Update failed checksum: %d\n", checksum);
    } else {
        console_printf("MIE FW Update success!\n");
    }
    display();
    maple_wait_for_ready();
    */

    // Request version, make sure we're running our updated code.
    char version[128];
    maple_request_version(version);
    console_printf("MIE version string: %s\n", version);
    display();
    
    maple_request_jvs_reset(0x01);
    maple_wait_for_ready();

    // Now, display the JVS IO version ID.
    int jvs_id_status = maple_request_jvs_id(0x01, version);
    if (jvs_id_status == 0) 
      console_printf("JVS IO ID: %s\n\n", version);
    else
      console_printf("JVS IO ID error: %d\n\n", jvs_id_status);
    display();


    uint32_t get_condition_payload = 1;

    // Now, read the controls forever.
    unsigned int reset_loc = console_loc;
    int liveness = 0;

    resp = maple_swap_data(1, 0, MAPLE_DEVICE_INFO_REQUEST, 0, NULL);
    console_printf("Maple Device 1 Info resp: ");
    for (int i = 0; i < 10; i++) {
        console_printf("%02X ", resp[i]);
    }
    console_printf("\n");

    resp = maple_swap_data(2, 0, MAPLE_DEVICE_INFO_REQUEST, 0, NULL);
    console_printf("Maple Device 2 Info resp: ");
    for (int i = 0; i < 10; i++) {
        console_printf("%02X ", resp[i]);
    }
    console_printf("\n");

    while ( 1 )
    {
        console_loc = reset_loc;
        console_printf("Liveness indicator: %d\n", liveness++);

        resp = maple_swap_data(1, 0, 0x09, 1, &get_condition_payload);
        console_printf("Maple Device 1 GetCond resp: ");
        for (int i = 0; i < 10; i++) {
            console_printf("%02X ", resp[i]);
        }
        console_printf("\n");

        resp = maple_swap_data(2, 0, 0x09, 1, &get_condition_payload);
        console_printf("Maple Device 2 GetCond resp: ");
        for (int i = 0; i < 10; i++) {
            console_printf("%02X ", resp[i]);
        }
        console_printf("\n");

        //char* hello = "hello there, world";
        char* hello = "llehht o,ererow !!dl";

        resp = maple_swap_data(1, 0, 0xA0, strlen(hello), hello);
        console_printf("Maple Device 1 SendString resp: ");
        for (int i = 0; i < 10; i++) {
            console_printf("%02X ", resp[i]);
        }
        console_printf("\n");

        display();
    }
}

void test()
{
    video_init_simple();

    video_fill_screen(rgb(48, 48, 48));
    video_draw_text(320 - 56, 236, rgb(255, 255, 255), "test mode stub");
    video_wait_for_vblank();
    video_display();

    while ( 1 ) { ; }
}

/**
 * Request the MIE upload a new binary and then execute it.
 *
 * Return 0 on success
 *        -1 on unexpected packet received
 *        -2 on bad memory written
 *        -3 on bad crc
 *        -4 on failure to boot code.
 */
