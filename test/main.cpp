#include <iostream>
#include <GTSL/Core.hpp>
#include <GTSL/SmartPointer.hpp>
#include <GTSL/Collections/Vector.hpp>
#include <GTSL/Window.hpp>
#include <GTSL/Log.hpp>
#include <GTSL/Id.hpp>

const char* ToString(GTSL::WindowEvents event)
{
	switch(event)
	{
    case GTSL::WindowEvents::Close: return "Close";
	case GTSL::WindowEvents::Keyboard: return "Keyboard";
	case GTSL::WindowEvents::Char: return "Char";
	case GTSL::WindowEvents::Resize: return "Resize";
	case GTSL::WindowEvents::WindowMoving: return "Window Moving";
	case GTSL::WindowEvents::MouseButton: return "Mouse Button";
	case GTSL::WindowEvents::MouseMove: return "Mouse Move";
	case GTSL::WindowEvents::Scroll: return "Scroll";
	}
    return "";
}

int main(int argc, char** argv)
{
    GTSL::Vector<int,GTSL::DefaultAllocatorReference> c;

	//GTSL::StringView d = "Test";

    //std::string e = d;
    std::cout << "Hello World! " << std::endl;

    glfwInit();
    GTSL::Window* window = new GTSL::Window("Test", { 800,600 });

	window->WindowDelegate.AddLambda([](GTSL::Window* win,GTSL::WindowEvents event,void* data)
    {
		std::cout << "Window Event: " << ToString(event) << std::endl;
    });

    while(!window->ShouldWindowClose())
    {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
