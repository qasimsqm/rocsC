#pragma once

template <typename EXCEPTION_T, class LAMBDA_T>
bool MaskException(LAMBDA_T fn)
{
    try
    {
        fn();
    }
    catch (EXCEPTION_T &)
    {
        return false;
    }

    return true;
}
