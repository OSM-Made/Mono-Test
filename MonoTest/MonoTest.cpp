#include "Common.h"
#include "MonoTest.h"

extern "C"
{
	int module_start()
	{
		klog("!! Hello World !!\n");

		Mono::Init();

		UI::Utilities::SetVersionString("5.05 Test");

		Notify("Mono Test: Loaded2!");

		return 0;
	}

	int module_stop()
	{
		Notify("Mono Test: Unloading...");

		return 0;
	}

	void _start()
	{

	}
}