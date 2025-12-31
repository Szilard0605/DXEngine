#include <stdio.h>

#include "Core/Window.h"
#include "Core/Application.h"

int main()
{
	Application app(1280, 720, "D3D11 Renderer");
	app.Run();
	return 0;
}
