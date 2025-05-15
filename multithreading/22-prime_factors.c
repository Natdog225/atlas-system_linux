#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * create_task - Allocates and initializes a task.
 * @entry: Task's entry function.
 * @param: Parameter for entry function.
 *
 * Return: Pointer to created task_t, or NULL on failure.
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *new_task;

	new_task = malloc(sizeof(task_t));
	if (!new_task)
		return (NULL);

	new_task->entry = entry;
	new_task->param = param;
	new_task->status = PENDING;
	new_task->result = NULL;

	if (pthread_mutex_init(&new_task->lock, NULL) != 0)
	{
		free(new_task);
		return (NULL);
	}
	return (new_task);
}

/**
 * destroy_task - Frees a task and its resources.
 * @task: Task to destroy.
 *
 * Description: Frees task's result, destroys its mutex,
 * and then frees the task structure itself. Used as list_destroy callback.
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;

	if (task->result != NULL)
	{
		list_destroy((list_t *)task->result, free);
		free(task->result);
	}

	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * exec_tasks - Thread entry to execute tasks from a list.
 * @tasks: Const list of tasks to execute.
 *
 * Description: Worker threads pick PENDING tasks from the list, execute them,
 * and log their status using tprintf. Threads exit when no PENDING
 * tasks remain after a full scan of the list.
 *
 * Return: Always NULL.
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *current_node;
	task_t *current_task;
	int task_idx;
	int processed_task_in_pass;

	if (!tasks)
		return (NULL);

	while (1)
	{
		processed_task_in_pass = 0;
		current_node = tasks->head;
		task_idx = 0;

		while (current_node != NULL)
		{
			current_task = (task_t *)current_node->content;

			pthread_mutex_lock(&current_task->lock);
			if (current_task->status == PENDING)
			{
				current_task->status = STARTED;
				pthread_mutex_unlock(&current_task->lock); /* Unlock before execution */

				tprintf("[%02d] Started\n", task_idx);
				current_task->result = current_task->entry(current_task->param);

				pthread_mutex_lock(&current_task->lock); /* Lock to update status */
				if (current_task->result != NULL)
				{
					current_task->status = SUCCESS;
					tprintf("[%02d] Success\n", task_idx);
				}
				else
				{
					current_task->status = FAILURE;
					tprintf("[%02d] Failure\n", task_idx);
				}
				pthread_mutex_unlock(&current_task->lock);
				processed_task_in_pass = 1;
			}
			else
			{
				pthread_mutex_unlock(&current_task->lock);
			}
			current_node = current_node->next;
			task_idx++;
		}

		if (!processed_task_in_pass) /* No PENDING tasks found in a full pass */
			break;					 /* Exit while(1) loop, thread terminates */
	}
	return (NULL);
}
