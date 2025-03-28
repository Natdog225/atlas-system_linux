#include <Python.h>
#include <stdio.h>
#include <object.h>
#include <listobject.h>

/**
 * print_python_list - Prints basic info about a Python list.
 * @p: A pointer to a PyObject list object.
 */
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
	}
	fflush(stdout);
}