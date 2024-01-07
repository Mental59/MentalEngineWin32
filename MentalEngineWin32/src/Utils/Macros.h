#pragma once

#include "Exceptions/ComException.h"

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
HRESULT hr__ = (x); \
if(FAILED(hr__)) { throw ComException(hr__, #x, __FILE__, __LINE__); } \
}
#endif // ThrowIfFailed

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif // ReleaseCom
