#include <Python.h>
#include <stdio.h>
#include <unicodeobject.h>
#include <bytesobject.h>

void print_python_string(PyObject *p)
{
	Py_ssize_t length;
	const char *type_str = NULL;
	PyObject *utf8_bytes = NULL;
	const char *value_str = NULL;

	printf("[.] string object info\n");
	fflush(stdout);

	if (!PyUnicode_Check(p))
	{
		printf("  [ERROR] Invalid String Object\n");
		fflush(stdout);
		return;
	}
	length = PyUnicode_GET_LENGTH(p);

	if (PyUnicode_IS_COMPACT_ASCII(p))
	{
		type_str = "compact ascii";
	}
	else
	{
		type_str = "compact unicode object";
	}

	utf8_bytes = PyUnicode_AsUTF8String(p);
	if (utf8_bytes && PyBytes_Check(utf8_bytes))
	{
		value_str = ((PyBytesObject *)utf8_bytes)->ob_sval;
	}
	printf("  type: %s\n", type_str);
	printf("  length: %zd\n", length);

	if (value_str)
	{
		printf("  value: %s\n", value_str);
	}
	else
	{
		printf("  value: <error retrieving value as UTF-8>\n");
	}
	Py_XDECREF(utf8_bytes);

	fflush(stdout);

}