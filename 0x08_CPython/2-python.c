#include <stdio.h>
#include <object.h>
#include <listobject.h>
#include <bytesobject.h>

void print_python_bytes(PyObject *p);
{
	PyBytesObject *bytes_obj;
	Py_ssize_t size, i, limit;
	char *data;

	printf("[.] bytes object info\n");
	/* Check if it's valid PyObject */
	if (!PyBytes_Check(p))
	{
		printf("  [ERROR] Invalid Byes Object\n");
		fflush(stdout);
		return;
	}
	bytes_obj = (PyBytesObject *)p;
	size = ((PyVarObject *)p)->ob_size;
	data = bytes_obj->ob_sval;

	printf("  size: %zd\n", size);

	printf("  trying string: %s\n", data);

	limit = size + 1;
	if (limit > 10)
	{
		limit = 10;
	}

	printf("  first %zd bytes:", limit);
	for (i = 0; i < limit; i++)
	{
		printf(" %02x", (unsigned char)data[i]);
	}
	printf("\n");
	fflush(stdout)
}

void print_python_list(PyObject *p)
{
	PyListObject *list = (PyListObject *)p;
	Py_ssize_t size, allocated, i;
	const char *typeName;

	size = ((PyVarObject *)p)->ob_size;
	allocated = list->allocated;

	printf("[*] Python list info\n");
	printf("[*] Size of the Python List = %zd\n", size);
	printf("[*] Allocated = %zd\n", allocated);

	for (i = 0; i < size; ++i)
	{
		PyObject *element = list->ob_item[i];

		typeName = element->ob_type->tp_name;

		printf("Element %zd: %s\n", i, typeName);

		if (PyBytes_Check(element))
		{
			print_python_bytes(element);
		}
	}
	fflush(stdout);
}