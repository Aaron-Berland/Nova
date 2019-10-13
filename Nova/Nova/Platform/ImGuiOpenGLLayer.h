#pragma once
#ifndef IMGUIOPENGLLAYER_H
#define IMGUIOPENGLLAYER_H

#include <Nova/ImGui/ImGuiLayer.h>

namespace Nova
{
	class NOVA_API ImGuiOpenGLLayer : public ImGuiLayer
	{
	public:
		virtual ~ImGuiOpenGLLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void Begin() override;
		virtual void End() override;
		ImGuiOpenGLLayer() = default;



	};

}

#endif