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

// Byte-swap strings to be 32-bit endian swapped
// Maple payloads apparently are only 32-bit little-endian
int convert_to_dma(char* input, char** payload) {
  int input_len = strlen(input); 
  //make sure buffer has enough room to hold string + be able to endian-swap the last chunk
  int buf_size = ((input_len + 1 + 3) & ~0x03); 

  *payload = malloc(buf_size);
  strcpy(*payload, input);
  for(int i = 0; i < buf_size; i+=4) {
    char c = (*payload)[i+0];
    (*payload)[i+0] = (*payload)[i+3];
    (*payload)[i+3] = c;

    char d = (*payload)[i+1];
    (*payload)[i+1] = (*payload)[i+2];
    (*payload)[i+2] = d;
  }

  return buf_size;
}

void send_serial(char* text) {
  uint32_t *resp;
  char* dma_string;

  int dma_len = convert_to_dma(text, &dma_string);

  resp = maple_swap_data(1, 0, 0xA0, dma_len/4, dma_string);
  console_printf("Maple Device 1 SendString resp: ");
  for (int i = 0; i < 10; i++) {
      console_printf("%02X ", resp[i]);
  }
  console_printf("\n");
  free(dma_string);
}

int get_buttons(jvs_buttons_t* buttons) {
  int button_pushed = 0;
  static jvs_buttons_t last_buttons;
  *buttons = maple_request_jvs_buttons(0x01, 2);

  if (memcmp(buttons, &last_buttons, sizeof(last_buttons)) != 0) {
    button_pushed = 1;
  }

  last_buttons = *buttons;
  return button_pushed;
}


void display(int y_line)
{
    // Render a simple test console.
    video_fill_screen(rgb(48, 48, 48));
    video_draw_text(0, 0, rgb(255, 255, 255), console_base);
    video_fill_box(300,(y_line*8),339,((y_line+1)*8)-1,rgb(255, 0, 0));
    video_wait_for_vblank();
    video_display();
}

void main()
{
    #define MENU_SECTION 0xC030000
    volatile unsigned char *menu_section = (volatile unsigned char *)MENU_SECTION;

    char* end_of_data = strchr((const char *)menu_section,'^'); // special signifier of end of data
    *end_of_data='\0'; //replace with null. Now we're a string

    uint32_t *resp;

    // Set up a crude console
    video_init_simple();
    maple_init();
    console_base = malloc(((640 * 480) / (8 * 8)) + 1);
    memset(console_base, 0, ((640 * 480) / (8 * 8)) + 1);

    console_printf("%s\n", menu_section);

    // Now, read the controls forever.
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

    jvs_buttons_t buttons;
    int y_line = 0;

    unsigned int reset_loc = console_loc;
    while ( 1 )
    {
        console_loc = reset_loc;
        console_printf("Liveness indicator: %d\n", liveness++);

        send_serial("Hello, World!");

        if (get_buttons(&buttons)) {
          console_printf("\n1P buttons: ");
          if(buttons.player1.start)
          {
              console_printf("start ");
          }
          if(buttons.player1.up)
          {
              console_printf("up ");
              if (y_line > 0)
                y_line--;
          }
          if(buttons.player1.down)
          {
              console_printf("down ");
              if(y_line < 40)
                y_line++;
          }
          if(buttons.player1.button1)
          {
              console_printf("b1 ");
          }
        }

        display(y_line);
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
