#include <stdlib.h>			/* For strtoul, malloc, free */
#include "multithreading.h" /* For list_t, node_t, list functions, prime_factors proto */

/**
 * prime_factors - Factorizes a number into a list of prime factors.
 * @s: String representation of the number to factorize.
 *
 * Return: Pointer to a list_t list containing prime factors (as u_long *).
 * Returns NULL on allocation failure or if s is invalid.
 */
list_t *prime_factors(char const *s)
{
	unsigned long n;
	char *endptr;
	list_t *factors;
	unsigned long d_factor;
	unsigned long *p_factor_val;

	n = strtoul(s, &endptr, 10);
	/* Problem implies valid positive number that fits in unsigned long. */
	/* Minimal check for empty or clearly invalid string for robustness. */
	if (s == NULL || *s == '\0' || (endptr != NULL && *endptr != '\0'))
	{
		factors = malloc(sizeof(list_t));
		if (!factors)
			return (NULL);
		list_init(factors); /* from list.c via multithreading.h */
		return (factors);	/* Return empty list for invalid input */
	}

	factors = malloc(sizeof(list_t));
	if (!factors)
		return (NULL);
	list_init(factors);

	while (n % 2 == 0) /* Factor out 2s */
	{
		p_factor_val = malloc(sizeof(unsigned long));
		if (!p_factor_val)
		{
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}
		*p_factor_val = 2;
		if (list_add(factors, p_factor_val) == NULL) /* list_add returns new node */
		{
			free(p_factor_val);
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}
		n /= 2;
	}
	for (d_factor = 3; d_factor * d_factor <= n; d_factor += 2) /* Odd factors */
	{
		while (n % d_factor == 0)
		{
			p_factor_val = malloc(sizeof(unsigned long));
			if (!p_factor_val)
			{
				list_destroy(factors, free);
				free(factors);
				return (NULL);
			}
			*p_factor_val = d_factor;
			if (list_add(factors, p_factor_val) == NULL)
			{
				free(p_factor_val);
				list_destroy(factors, free);
				free(factors);
				return (NULL);
			}
			n /= d_factor;
		}
	}
	if (n > 1) /* Remaining n is prime */
	{
		p_factor_val = malloc(sizeof(unsigned long));
		if (!p_factor_val)
		{
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}
		*p_factor_val = n;
		if (list_add(factors, p_factor_val) == NULL)
		{
			free(p_factor_val);
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}
	}
	return (factors);
}
