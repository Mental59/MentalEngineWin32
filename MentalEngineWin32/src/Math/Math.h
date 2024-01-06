#pragma once

#include <windows.h>
#include <DirectXMath.h>

namespace Math
{
	inline bool VerifySupport()
	{
		return DirectX::XMVerifyCPUSupport();
	}
}
