#include <Python.h>
#include <stdio.h>
#include <longintrepr.h>
#include <limits.h>

void print_python_int(PyObject *p)
{
	PyLongObject *plong;
	Py_ssize_t size;
	Py_ssize_t abs_size;
	Py_ssize_t i;
	unsigned long uval = 0; 
	const unsigned long SHIFT = PyLong_SHIFT;

	if (!PyLong_Check(p))
	{
		printf("Invalid Int Object\n");
		fflush(stdout);
		return;
	}
	plong = (PyLongObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	abs_size = size < 0 ? -size : size;

	if (abs_size == 0)
		{
			printf("0\n");
			fflush(stdout);
			return;
		}

	const Py_ssize_t max_possible_ulong_digits = (sizeof(unsigned long) * CHAR_BIT +
	SHIFT - 1) / SHIFT;
	if (abs_size > max_possible_ulong_digits)
	{
		printf("C unsigned long int overflow\n");
		fflush(stdout);
		return;
	}

	uval = 0;

	for (i = abs_size -1; i >= 0; --i)
	{
		digit current_digit = plong->ob_digit[i];

		if (uval > (ULONG_MAX >> SHIFT))
		{
			printf("C unsigned long int overflow\n");
			fflush(stdout);
			return;
		}
		unsigned long shifted_uval = uval << SHIFT;

		if (current_digit > ULONG_MAX - shifted_uval)
		{
			printf("C unsigned long int overflow\n");
			fflush(stdout);
			return;
		}
		uval = shifted_uval + current_digit;
	}
	if (size < 0)
	{
		print("-%lu\n", uval);
	}
	else
	{
		printf("%lu\n", uval);
	}
	fflush(stdout);
}