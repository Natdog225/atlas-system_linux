#include "laps.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * struct car_s - Structure to represent a car
 * @id: identifier for the car
 * @laps: Number of laps completed by the car
 * @next: Pointer to the next car in the list
 */
typedef struct car_s
{
	int id;
	int laps;
	struct car_s *next;
} car_t;

static car_t *cars = NULL;

/**
 * add_car - Adds a new car to the linked list
 * @id: Identifier of the car
 *
 * Return: Pointer to the newly added car
 */
car_t *add_car(int id)
{
	car_t *new_car = malloc(sizeof(car_t));
	car_t *current = cars;

	if (new_car == NULL)
		return (NULL);

	new_car->id = id;
	new_car->laps = 0;
	new_car->next = NULL;

	if (cars == NULL)
	{
		cars = new_car;
	}
	else
	{
		while (current->next != NULL)
			current = current->next;
		current->next = new_car;
	}

	printf("Car %d joined the race\n", id);
	return (new_car);
}

/**
 * update_laps - Updates the number of laps for a car
 * @id: Identifier of the car
 */
void update_laps(int id)
{
	car_t *current = cars;

	while (current != NULL)
	{
		if (current->id == id)
		{
			current->laps += 1;
			return;
		}
		current = current->next;
	}
}

/**
 * print_race_state - Prints the leaderboard
 */
void print_race_state(void)
{
	car_t *current = cars;

	printf("Race state:\n");
	while (current != NULL)
	{
		printf("Car %d [%d laps]\n", current->id, current->laps);
		current = current->next;
	}
}

/**
 * free_cars - Frees the memory allocated for the cars
 */
void free_cars(void)
{
	car_t *current = cars;
	car_t *temp;

	while (current != NULL)
	{
		temp = current;
		current = current->next;
		free(temp);
	}
	cars = NULL;
}

/**
 * race_state - Keeps track of cars in a race
 * @id: Array of car identifiers
 * @size: Size of the array
 */
void race_state(int *id, size_t size)
{
	size_t i;

	if (size == 0)
	{
		free_cars();
		return;
	}

	for (i = 0; i < size; i++)
	{
		car_t *current = cars;
		int found = 0;

		while (current != NULL)
		{
			if (current->id == id[i])
			{
				found = 1;
				break;
			}
			current = current->next;
		}

		if (!found)
			add_car(id[i]);
		else
			update_laps(id[i]);
	}

	print_race_state();
}
