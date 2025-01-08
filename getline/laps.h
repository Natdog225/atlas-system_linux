#ifndef LAPS_H
#define LAPS_H

#include <stddef.h>

/**
 * struct car_s - Structure to represent a car in the race
 * @id: Unique identifier for the car
 * @laps: Number of laps completed by the car
 * @next: Pointer to the next car in the linked list
 */
typedef struct car_s
{
	int id;
	int laps;
	struct car_s *next;
} car_t;

void race_state(int *id, size_t size);

#endif /* LAPS_H */
