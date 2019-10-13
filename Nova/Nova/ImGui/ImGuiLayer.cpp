#include "ImGuiLayer.h"


/*
	TODO(BYRON) : This should change once the engine supports multiple platforms
*/
#include <Nova/Platform/ImGuiOpenGLLayer.h>

namespace Nova
{




	ImGuiLayer* CreateImGuiLayer()
	{
		return new ImGuiOpenGLLayer();
	}
	
}