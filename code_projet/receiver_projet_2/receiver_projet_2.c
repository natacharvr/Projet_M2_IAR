/**
 * Natacha Rivière
**/


/** \file
receiver side.

Robot B continuously listen in each direction.


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

#define HASH_TABLE_SIZE 5

typedef struct Node {
    uint32_t timepoint;
    int count_one_third;
    int count_two_third;
    int count_max;
} Node;

typedef struct NodeTable {
    Node hash_table[HASH_TABLE_SIZE][HASH_TABLE_SIZE];
} NodeTable;


void initializeHashTable(NodeTable *table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (int j = 0; j < HASH_TABLE_SIZE; j++) {
            table->hash_table[i][j].timepoint = -1;
            table->hash_table[i][j].count_one_third = 0;
            table->hash_table[i][j].count_two_third = 0;
            table->hash_table[i][j].count_max = 0;
        }
    }
}


unsigned int my_hash(int timepoint) {
    return (timepoint) % HASH_TABLE_SIZE;
}

bool increment_timepoint(uint32_t timepoint, int power_level, NodeTable* table) {
    unsigned int index = my_hash(timepoint);
    bool assigned = false;
    int index2 = 0;
    while ((assigned == false) && (index2 < HASH_TABLE_SIZE)) {
        Node *current = &table->hash_table[index][index2];
        if ((current->timepoint == -1) || (current->timepoint == timepoint)) {
            current->timepoint = timepoint;
            if (power_level == 1) {
                current->count_one_third += 1;
            } else if (power_level == 2) {
                current->count_two_third += 1;
            } else {
                current->count_max += 1;
            }
            assigned = true;
        }
        index2 ++;
    }
    return assigned;
}

void display_table(NodeTable* table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        for (int j = 0; j < HASH_TABLE_SIZE; j++) {
            Node *current = &table->hash_table[i][j];
            if (current->timepoint != -1) {
                printf("timepoint: %ld, count_one_third: %d, count_two_third: %d, count_max: %d\n", current->timepoint, current->count_one_third, current->count_two_third, current->count_max);
            }
        }
    }
}

uint32_t get_timepoint(unsigned char * payload) {
    uint32_t timepoint = 0;
    int i = 2;
    while (payload[i] != '\0') {
        timepoint = timepoint * 10 + (payload[i] - '0');
        i++;
    }
    return timepoint/1000000;
}


int main(void) {

    pogobot_init();

    printf("init ok\n");

    pogobot_led_setColor( 25, 25, 25 );

    NodeTable table;
    initializeHashTable(&table);
    bool room_available = true;
    bool far_away = true;
    while (far_away) {
        initializeHashTable(&table);

        while (room_available)
        {
            pogobot_infrared_update();

            /* read reception fifo buffer */
            if ( pogobot_infrared_message_available() )
            {
                // pogobot_motor_set(motorL, motorStop);

                while ( pogobot_infrared_message_available() )
                {
                    message_t mr;
                    pogobot_infrared_recover_next_message( &mr );
                    if (mr.header._receiver_ir_index != 2) {
                        continue;
                    }

                    int power_level = mr.payload[0] - '0';
                    printf("color_index: %d\n", power_level);
                    const rgb8_t *const color = &( colors[power_level] );

                    pogobot_led_setColor( color->r, color->g, color->b );
                    printf( "RECV: len %d at %d [%s]\n", mr.header.payload_length, mr.header._receiver_ir_index,
                            mr.payload );
                    printf("%ld\n", get_timepoint(mr.payload));
                    room_available = increment_timepoint(get_timepoint(mr.payload), power_level, &table);
                    msleep( 10 );

                }
            } else {
                    pogobot_motor_set(motorL, motorFull);
                    msleep( 10 );
                    pogobot_motor_set(motorL, motorStop);
            }
        }
        printf("fin d'acquisition\n");
        display_table(&table);

        float mean_level_3 = 0;
        float mean_level_2 = 0;
        float mean_level_1 = 0;
        int filled_cell = 0;

        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            for (int j = 0; j < HASH_TABLE_SIZE; j++) {
                Node *current = &table.hash_table[i][j];
                if (current->timepoint != -1) {
                    mean_level_3 += current->count_max;
                    mean_level_2 += current->count_two_third;
                    mean_level_1 += current->count_one_third;
                    filled_cell ++;
                }
                
            }
        }
        mean_level_3 = mean_level_3 / filled_cell;
        mean_level_2 = mean_level_2 / filled_cell;
        mean_level_1 = mean_level_1 / filled_cell;

        printf("mean_level_3: %d, mean_level_2: %d, mean_level_1: %d\n", (int)mean_level_3*100, (int)mean_level_2*100, (int)mean_level_1*100);
        // printf("%d", (int)mean_level_3*100);

        if (mean_level_3 - mean_level_1 > 1.f) {
            // far_away = false;
            return 0;
        }
        room_available = true;
    }
    return 0;

}
