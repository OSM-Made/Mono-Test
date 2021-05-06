#include "Common.h"
#include "MonoTest.h"
#include "Settings_Menu.h"

extern "C"
{
	int module_start()
	{
		klog("!! Hello World !!\n");

		Mono::Init();

		UI::Utilities::SetVersionString("5.05 Test");
		Settings_Menu::Init();
		//Title_Menu::Init();

		Notify("Mono Test: Loaded!");

		return 0;
	}

	int module_stop()
	{
		Notify("Mono Test: Unloading...");

		Settings_Menu::Term();
		//Title_Menu::Term();

		return 0;
	}

	void _start()
	{

	}
}