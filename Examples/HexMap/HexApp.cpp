#include <Nova.h>
#include "HexLayer.h"

constexpr unsigned int w = 1600;
constexpr unsigned int h = 900;

class HexApp : public Nova::Application
{
public:
	HexApp(const Nova::WindowProperties& props) : Nova::Application(props)
	{
		PushLayer(new HexLayer());

	}
	~HexApp() = default;
};


Nova::Application* Nova::CreateApplication()
{
	Nova::WindowProperties props;
	props.Title = "HexMap";
	props.Height = h;
	props.Width = w;
	return new HexApp(props);
}