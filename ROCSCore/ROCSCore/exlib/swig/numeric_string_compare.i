%import "exlib/numeric_string_compare.h"

%typemap(out) ex::SortedStringMapT
{
    PyObject *d = PyDict_New();
    
    for (auto it: $1)
    {
        PyDict_SetItemString(d, it.first.c_str(), PyString_FromString(it.second.c_str()));
    }

    $result = d;
}
