#pragma once
// General includes and defnitions that might be needed in a number of places

// NOMINMAX is defined because windows.h provides definitions for the functions min and max
// by default. If they are included, this can cause problems with the use of min and max in
// the standard library.  This causes some problems in later code, so we just include it from
// the start
#define NOMINMAX

#include <windows.h>
#include "Resource.h"
#include <string>
#include <exception>

using namespace std;

// Used with some DirectX calls to convert the return of a failure error code
// into an exception 

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw exception();
	}
}

