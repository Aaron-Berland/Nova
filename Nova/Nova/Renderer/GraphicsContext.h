#pragma once
#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

namespace Nova
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}

#endif