/**
 * POGOBOT
 * Natacha Rivière, Léa Movsessian
**/


/** \file
sender side.
Continuoulsy sends messages at all power levels
each message contains the power level and the timestamp at which it was sent
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

    int direction = 0;

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


            // send at front and back to avoid weird alignments but cover enough angles
            pogobot_infrared_sendRawShortMessage(
                direction, &mes );
            pogobot_infrared_sendRawShortMessage(
                (direction+2)%2, &mes );

        msleep( 10 );
        }
        
        // this does snot work well, when the robots are very close, the emitter does not receive the messages
        pogobot_infrared_update();

        if (pogobot_infrared_message_available()) {
            while (pogobot_infrared_message_available()) {
                message_t mr;
                pogobot_infrared_recover_next_message(&mr);
                printf("RECV: len %d at %d [%s]\n", mr.header.payload_length, mr.header._receiver_ir_index, mr.payload);
                int used_direction = mr.header._receiver_ir_index;
                if (used_direction == direction) {
                    direction ++;
                    if (direction > 3) {
                        return 0;
                    }
                }
            }
        }
    }

}

