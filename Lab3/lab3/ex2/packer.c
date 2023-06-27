#include "packer.h"
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#define COLOURS_MAPPING 3

// You can declare global variables here
sem_t color_semaphores[COLOURS_MAPPING];
sem_t full_semaphores[COLOURS_MAPPING];
int packing_max;

pthread_mutex_t packing_area_mutex;

struct packing_area
{
    int color_ids[COLOURS_MAPPING][64];
    int color_count[COLOURS_MAPPING];

} packing_area;

void packer_init(int balls_per_pack)
{
    // Write initialization code here (called once at the start of the program).
    for (int i = 0; i < COLOURS_MAPPING; i++)
    {
        sem_init(&color_semaphores[i], 0, 0);
        sem_init(&full_semaphores[i], 0, 0);
    }

    packing_max = balls_per_pack;

    // initialise the useful index to -1 (Represents Empty)
    for (int i = 0; i < COLOURS_MAPPING; i++)
    {
        for (int j = 0; j < packing_max; j++)
        {
            packing_area.color_ids[i][j] = -1;
        }
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
        sem_destroy(&full_semaphores[i]);
    }

    // Destroy Mutex
    pthread_mutex_destroy(&packing_area_mutex);
}

// Cases are: 1. A ball of same colour enters -> release the 2 balls and return the previous ball id
//            2. A ball of different colour enters -> store this ball
void pack_ball(int colour, int id, int *other_ids)
{
    // Get Corresponding color semaphore
    sem_t *color_sem = &color_semaphores[colour - 1];
    sem_t *done_packing_sem = &full_semaphores[colour - 1];

    // Get Mutex to enter packing area
    pthread_mutex_lock(&packing_area_mutex);

    // Keep track of the number of same coloured balls
    packing_area.color_count[colour - 1]++;

    // Store the ID on next available index
    for (int i = 0; i < packing_max; i++)
    {
        if (packing_area.color_ids[colour - 1][i] == -1)
        {
            packing_area.color_ids[colour - 1][i] = id;
            break;
        }
    }

    // If last ball
    if (packing_area.color_count[colour - 1] == packing_max)
    {
        // printf("I'm in last ball area %d", id);
        // Last packing
        int return_index = 0;

        for (int i = 0; i < packing_max; i++)
        {
            if (packing_area.color_ids[colour - 1][i] != id)
            {
                other_ids[return_index] = packing_area.color_ids[colour - 1][i];
                return_index++;
            }
            continue;
        }

        packing_area.color_count[colour - 1]--;

        sem_post(color_sem);
        sem_wait(done_packing_sem);
        // Reset
        for (int i = 0; i < packing_max; i++)
        {
            packing_area.color_ids[colour - 1][i] = -1;
        }
        pthread_mutex_unlock(&packing_area_mutex);
    }
    else
    {
        // printf("I'm in non last ball area %d", id);
        pthread_mutex_unlock(&packing_area_mutex);
        sem_wait(color_sem);
        // Pack as usual
        int return_index = 0;

        for (int i = 0; i < packing_max; i++)
        {
            if (packing_area.color_ids[colour - 1][i] != id)
            {
                other_ids[return_index] = packing_area.color_ids[colour - 1][i];
                return_index++;
            }
            continue;
        }

        packing_area.color_count[colour - 1]--;
        if (packing_area.color_count[colour - 1] == 0)
        {
            sem_post(done_packing_sem);
        }
        else
        {
            sem_post(color_sem);
        }
    }
}