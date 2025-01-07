/**
 * Natacha Rivière
**/


/** \file
receiver side.

This file implements the receiver side of demo "D2.1 Testing
communication: introverted listener".

It exercises the following features: RGB LED, low-level infrared
transmission API.

Details:

Robot B continuously listen in each direction.
- for each message, change RGB LED according to which of its own IR received a
message:
- Robot A’s front: red
- Robot A’s left: green
- Robot A’s back: blue
- Robot A’s right: yellow
- No message: cyan/black

*/

/* clang-format-ok */

#include <stdio.h>
#include <string.h>
#include "pogobot.h"


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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 100

typedef struct Node {
    int timepoint;
    int count_one_third;
    int count_two_third;
    int count_max;
    struct Node* next;
} Node;

Node* hash_table[HASH_TABLE_SIZE] = {NULL};

unsigned int my_hash(int timepoint) {
    return timepoint % HASH_TABLE_SIZE;
}

void increment_timepoint(int timepoint, int power_level) {
    unsigned int index = my_hash(timepoint);
    Node* current = hash_table[index];

    while (current) {
        if (current->timepoint == timepoint) {
            if (power_level == 1) {
                current->count_one_third++;
            } else if (power_level == 2) {
                current->count_two_third++;
            } else {
                current->count_max++;
            }
            return;
        }
        current = current->next;
    }

    // Node* new_node = (Node*)malloc(sizeof(Node));
    Node* new_node;

    new_node->timepoint = timepoint;
    if (power_level == 1) {
        new_node->count_one_third = 1;
        new_node->count_two_third = 0;
        new_node->count_max = 0;
    } else if (power_level == 2) {
        new_node->count_one_third = 0;
        new_node->count_two_third = 1;
        new_node->count_max = 0;
    } else {
        new_node->count_one_third = 0;
        new_node->count_two_third = 0;
        new_node->count_max = 1;
    }

    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

// void display_counts() {
//     for (int i = 0; i < HASH_TABLE_SIZE; i++) {
//         Node* current = hash_table[i];
//         while (current) {
//             printf("Timepoint %d: %d messages\n", current->timepoint, current->count);
//             current = current->next;
//         }
//     }
// }


int main(void) {

    pogobot_init();

    printf("init ok\n");

    pogobot_led_setColor( 25, 25, 25 );

    while (1)
    {
        pogobot_infrared_update();

        /* read reception fifo buffer */
        if ( pogobot_infrared_message_available() )
        {
            while ( pogobot_infrared_message_available() )
            {
                message_t mr;
                pogobot_infrared_recover_next_message( &mr );

                int color_index = mr.payload[0] - '0';
                printf("color_index: %d\n", color_index);
                const rgb8_t *const color = &( colors[color_index] );

                pogobot_led_setColor( color->r, color->g, color->b );
                printf( "RECV: len %d [%s]\n", mr.header.payload_length,
                        mr.payload );
            }
        }
        // else
        // {
        //     const rgb8_t *const color = &( colors[4] );
        //     // pogobot_led_setColor( color->r, color->g, color->b );
        // }
    }

}
