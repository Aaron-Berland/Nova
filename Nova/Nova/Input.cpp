#include "Input.h"





/*
	TODO(BYRON): Temporary while not supporting multiples platforms
*/
#include <Nova/Platform/GLFWInput.h>
namespace Nova
{
	Scope<Input> Input::s_instance = CreateScope<GLFWInput>();
}