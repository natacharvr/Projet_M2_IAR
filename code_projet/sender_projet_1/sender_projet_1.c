/**
 * POGOBOT
 * Natacha Rivière
**/


/** \file
Pogobot demo source code D2.3. (sender code)

It exercises the following features: RGB LED API, low-level infrared
transmission API, stopwatch API.

Details:

- Two robots (A and B) are not moving and located next to one another (close
enough for IR to work)
- Robot A continuously emits one specific message in all directions (~broadcast)
    - the content of the message changes every two seconds
    - a message contains:
        -name of robot (“robot A”)
        -a RGB value out of #FF0000, #00FF00, #0000FF, #FF00FF, #FFFF00, #00FFFF
    -RBG LED matches the message’s RGB values (i.e. also changes every two
seconds)


Testing protocol: move Robot B by hand to test all combinations, including “no
message”.

*/

/* clang-format-ok */

#include <stdio.h>
#include <string.h>
#include "pogobot.h"


#define message_length_bytes ( sizeof(sizeof(char)*2 + sizeof(int32_t)))

unsigned char *messages[4] = {
    (unsigned char *) "0",
    (unsigned char *) "1",
    (unsigned char *) "2",
    (unsigned char *) "3",
};

typedef struct rgb8_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb8_t;

const rgb8_t colors[] = {
    { .r = 0x80, .g = 0x00, .b = 0x00 }, // power_level 0
    { .r = 0x00, .g = 0x80, .b = 0x00 }, // power_level 1
    { .r = 0x00, .g = 0x00, .b = 0x80 }, // power_level 2
    { .r = 0x80, .g = 0x80, .b = 0x00 }, // power_level 3
    { .r = 0x00, .g = 0x80, .b = 0x80 }, // none
};


int main(void) {

    pogobot_init();

    printf("init ok\n");

    pogobot_led_setColor( 25, 25, 25 );
    time_reference_t mystopwatch;
    pogobot_stopwatch_reset( &mystopwatch );
    
    while (1)
    {
        for (int power_level = 0; power_level < 4; power_level ++) {
            pogobot_infrared_set_power(power_level);
            const rgb8_t *const color = &( colors[power_level] );

            pogobot_led_setColor( color->r, color->g, color->b );

            pogobot_infrared_update();

            short_message_t mes;
            sprintf((char*)(mes.payload), "%d %lu", power_level, pogobot_stopwatch_get_elapsed_microseconds(&mystopwatch));
            // mes.header.payload_length = message_length_bytes;
            mes.header.payload_length = strlen((char*)(mes.payload)) +1;

            printf( "TRANS: %s, len = %d \n", mes.payload, mes.header.payload_length );

            pogobot_infrared_sendRawShortMessage(
                0, &mes );

        msleep( 100 );
        }
    }

}

