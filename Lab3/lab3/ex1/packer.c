#include "packer.h"
#include <semaphore.h>
#include <pthread.h>

#define COLOURS_MAPPING 3
#define PACKING_NUM 2

// You can declare global variables here
sem_t color_semaphores[COLOURS_MAPPING];

pthread_mutex_t packing_area_mutex;

int list_id[COLOURS_MAPPING][PACKING_NUM];
int colours_count[COLOURS_MAPPING];

void packer_init(void)
{
    for (int i = 0; i < COLOURS_MAPPING; i++)
    {
        sem_init(&color_semaphores[i], 0, 0);
    }
    // Initialise Mutex
    pthread_mutex_init(&packing_area_mutex, NULL);
}

void packer_destroy(void)
{
    // Write deinitialization code here (called once at the end of the program).
    for (int i = 0; i < COLOURS_MAPPING; i++)
    {
        sem_destroy(&color_semaphores[i]);
    }
 
    // Destroy Mutex
    pthread_mutex_destroy(&packing_area_mutex);
}

// Cases are: 1. A ball of same colour enters -> release the 2 balls and return the previous ball id
//            2. A ball of different colour enters -> store this ball
int pack_ball(int colour, int id)
{
    int return_id = 0;

    // Get Corresponding color semaphore
    sem_t *color_sem = &color_semaphores[colour - 1];

    // Get Mutex to enter packing area
    pthread_mutex_lock(&packing_area_mutex);

    if (colours_count[colour - 1] == 0)
    {

        // Store the ID of the ball
        list_id[colour - 1][0] = id;
        // Keep track of the number of same coloured balls
        colours_count[colour - 1]++;

        pthread_mutex_unlock(&packing_area_mutex);

        sem_wait(color_sem);

        return_id = list_id[colour - 1][1];

        // reset the id to -1
        list_id[colour - 1][0] = -1;
        list_id[colour - 1][1] = -1;
        // reset colour_count to 0
        colours_count[colour - 1] = 0;
    }
    else
    { // count is already 1

        return_id = list_id[colour - 1][0];

        list_id[colour - 1][1] = id;

        sem_post(color_sem);

        pthread_mutex_unlock(&packing_area_mutex);
    }

    return return_id;
}