/** exlib_xplatform_typemaps.i **/

%import "exlib/xplatform_types.h"

%define UNSIGNED_HELPER(type, max_val)

%typemap(in) type
{
    if (!PyNumber_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "argument must be a number");
        return NULL;
    }
    
    long long val = PyLong_AsLongLong($input); // raises OverflowError
    if (val < 0) {
        PyErr_SetString(PyExc_ValueError, "argument must be nonnegative");
        return NULL;
    }
    if (static_cast<unsigned long long>(val) > max_val) {
        PyErr_SetString(PyExc_OverflowError, "argument causes overflow");
        return NULL;
    }
    $1 = (type)val;
}

%typecheck(SWIG_TYPECHECK_INTEGER) type
{
    $1 = (PyLong_CheckExact($input) || PyInt_CheckExact($input));
}

%enddef

%define SIGNED_HELPER(type, max_val, min_val)

%typemap(in) type
{
    if (!PyNumber_Check($input)) {
        PyErr_SetString(PyExc_TypeError, "argument must be a number");
        return NULL;
    }
    
    long long val = PyLong_AsLongLong($input); // raises OverflowError
    if ((val > max_val) || (val < min_val)) {
        PyErr_SetString(PyExc_OverflowError, "argument causes overflow");
        return NULL;
    }
    $1 = (type)val;
}

%typecheck(SWIG_TYPECHECK_INTEGER) type
{
    $1 = (PyLong_CheckExact($input) || PyInt_CheckExact($input));
}

%enddef

UNSIGNED_HELPER(unsigned char, MAX_UINT8)
UNSIGNED_HELPER(unsigned short, MAX_UINT16)
UNSIGNED_HELPER(unsigned int, MAX_UINT32)
UNSIGNED_HELPER(unsigned long, MAX_UINT64)
SIGNED_HELPER(SInt8, MAX_SINT8, MIN_SINT8)
SIGNED_HELPER(short, MAX_SINT16, MIN_SINT16)
SIGNED_HELPER(int, MAX_SINT32, MIN_SINT32)
SIGNED_HELPER(long, MAX_SINT64, MIN_SINT64)

%typemap(out)   unsigned char,
                SInt8,
                unsigned short, 
                short,
                unsigned int,
                int
{
    PyObject *o = PyInt_FromLong((long)$1);
    $result = o;
}

%typemap(out) unsigned long
{
    $result = PyLong_FromUnsignedLong($1);
}

%apply unsigned char    { UInt8 };
%apply unsigned short   { UInt16 };
%apply unsigned int     { UInt32 };
%apply unsigned long    { UInt64 };
%apply short            { SInt16 };
%apply int              { SInt32 };
%apply long             { SInt64 };

%typecheck(SWIG_TYPECHECK_FLOAT) Float32, Float64
{
    $1 = PyFloat_Check($input); 
}

%typemap(in) Float64
{
    double value = PyFloat_AsDouble($input);
    if (value == -1.0)
    {
        if (PyErr_Occurred())
        {
            return NULL;
        }
    }

    $1 = value;
}

%typemap(in) Float32
{
    double value = PyFloat_AsDouble($input);
    if (value == -1.0)
    {
        if (PyErr_Occurred())
        {
            return NULL;
        }
    }

    $1 = static_cast<Float32>(value);
}

%typemap(out) Float64
{
    $result = PyFloat_FromDouble($1);
}

%typemap(out) Float32
{
    $result = PyFloat_FromDouble(static_cast<Float32>($1));
}
