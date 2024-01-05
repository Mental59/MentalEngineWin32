#pragma once

#include <windows.h>
#include <DirectXMath.h>

namespace Graphics
{
	bool CheckDirectXMathSupport()
	{
		return DirectX::XMVerifyCPUSupport();
	}
}
