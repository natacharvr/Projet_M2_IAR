/**
 * Natacha Rivière
**/


/** \file

receiver side.

Robot B listens to the messages sent by robot A.
the bot moves randomly at first and then towards the other bot when it receives messages.

It counts the number of messages received of each power_level and determines if they are close or not.
If the bot is far from the other, it tries to advance towards it

When the bot is close to the emittor, it warns them and becomes an emittor to attract other bots
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 4
#define TIME_TO_CHANGE_POS 600

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

    pogobot_led_setColor( 231, 78, 152 );

    NodeTable table;
    initializeHashTable(&table);
    bool room_available = true;
    bool far_away = true;
    int count = 0;
    while (far_away) {
        initializeHashTable(&table);

        while (room_available)
        {
            pogobot_infrared_update();

            /* read reception fifo buffer */
            if ( pogobot_infrared_message_available() )
            {
                count = 0;
                while ( pogobot_infrared_message_available() )
                {
                    message_t mr;
                    pogobot_infrared_recover_next_message( &mr );
                    if (mr.header._receiver_ir_index != 0) { // has to be front to advance to other bot
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
                count ++;
                pogobot_motor_power_set(motorL, motorFull);
                msleep( 10 );
                pogobot_motor_power_set(motorL, motorStop);
                msleep( 10 );
            }
            if (count > TIME_TO_CHANGE_POS) { // change position if no message in about 6 seconds
                pogobot_motor_power_set(motorL, motorFull);
                pogobot_motor_power_set(motorR, motorFull);
                msleep( 500 );
                pogobot_motor_power_set(motorL, motorStop);
                pogobot_motor_power_set(motorR, motorStop);
                count = 0;
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
        printf("%d", (int)(mean_level_1 - mean_level_3)*100);

        if (mean_level_1 - mean_level_3 > 1.f) { // TODO : finetune the threshold
            far_away = false;
            continue;
            // return 0;
        }
        else { // advance towards the other robot
            pogobot_motor_power_set(motorL, motorFull);
            pogobot_motor_power_set(motorR, motorFull);
            msleep( 100 );
            pogobot_motor_power_set(motorL, motorStop);
            pogobot_motor_power_set(motorR, motorStop);

            // remove messages received during the movement
            pogobot_infrared_clear_message_queue() ;
            msleep( 100 );
        }
        room_available = true;
    }
    // send messages to warn the other robot that we are close
    for (int i = 0; i < 10; i++) {
        short_message_t mes;
        sprintf((char*)(mes.payload), "close");
        mes.header.payload_length = strlen((char*)(mes.payload)) +1;
        pogobot_infrared_set_power(1);
        pogobot_infrared_sendRawShortMessage(
            0, &mes );
        printf("close\n");
        msleep( 100 );
        pogobot_led_setColor( 231, 78, 152 );
    }
    msleep( 10000 );
    printf("Je deviens un sender !!\n");

    // _______________________ SENDER ________________________
    pogobot_init();

    printf("init ok\n");

    int direction = 2;

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
                direction, &mes );
            pogobot_infrared_sendRawShortMessage(
                (direction+2)%4, &mes );

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

