#include "Common.h"
#include "Settings_Menu.h"

#include "UI.h"

//Embedded xmls
extern uint8_t settings_root[];
extern int32_t settings_root_Size;
extern uint8_t test_menu[];
extern int32_t test_menu_Size;
extern uint8_t external_hdd[];
extern int32_t external_hdd_Size;

//Detours
Detour* Settings_Menu::Detour_GetManifestResourceStream = nullptr;
Detour* Settings_Menu::Detour_OnCheckVisible = nullptr;
Detour* Settings_Menu::Detour_OnPreCreate = nullptr;
Detour* Settings_Menu::Detour_OnPageActivating = nullptr;
Detour* Settings_Menu::Detour_OnPress = nullptr;

MonoObject* New_MemoryStream(void* Buffer, int Buffer_Size)
{
	MonoArray* Array = Mono::New_Array(mono_get_byte_class(), Buffer_Size);
	char* Array_addr = mono_array_addr_with_size(Array, sizeof(char), 0);
	memcpy(Array_addr, Buffer, Buffer_Size);

	MonoClass* MemoryStream = Mono::Get_Class(Mono::mscorlib, "System.IO", "MemoryStream");
	MonoObject* MemoryStream_Instance = Mono::New_Object(MemoryStream);
	Mono::Invoke<void>(Mono::mscorlib, MemoryStream, MemoryStream_Instance, ".ctor", Array, true);

	return MemoryStream_Instance;
}

uint64_t Settings_Menu::GetManifestResourceStream_Hook(uint64_t inst, MonoString* FileName)
{
	char* str = mono_string_to_utf8(FileName);
	klog("****\nFileName: %s\n****\n", str);

	if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.SettingsRoot.data.settings_root.xml"))
		return (uint64_t)New_MemoryStream(settings_root, settings_root_Size);
	else if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.TestMenu.xml"))
		return (uint64_t)New_MemoryStream(test_menu, test_menu_Size);
	else if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.external_hdd.xml"))
		return (uint64_t)New_MemoryStream(external_hdd, external_hdd_Size);
	else
		return Detour_GetManifestResourceStream->Stub<uint64_t>(inst, FileName);
}

void Settings_Menu::OnCheckVisible_Hook(MonoObject* Instance, MonoObject* element, MonoObject* e)
{
	if (Instance && element)
	{
		MonoClass* SettingElement = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingElement");
		char* Id = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingElement, "Id", element));
		if (!strcmp(Id, "id_message"))
			Mono::Set_Property(SettingElement, "Visible", element, false);
	}
	//    klog("OnVisible: %s\n", mono_string_to_utf8(Mono::Get_Property<MonoString*>(Mono::Get_Class(App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingElement"), "Id", element)));

	//Detour_OnCheckVisible->Stub<void>(Instance, element, e);
	//no real use unless we want to hide some parts of the menu for like say an advanced mode.
}

void Settings_Menu::OnPreCreate_Hook(MonoObject* Instance, MonoObject* element, MonoObject* e)
{
	if (Instance && element)
	{
		MonoClass* SettingElement = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingElement");
		MonoClass* ElementData = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "ElementData");
		char* Id = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingElement, "Id", element));
		//klog("OnPreCreate: %s\n", Id);

		if (!strcmp(Id, "id_enable_debug_settings"))
			Mono::Set_Property(SettingElement, "Value", element, Mono::New_String("1"));
		else if (!strcmp(Id, "id_system_disp_devkit_panel"))
			Mono::Set_Property(SettingElement, "Value", element, UI::DevkitPanel::ShowPanel ? Mono::New_String("1") : Mono::New_String("0"));
		else if (!strcmp(Id, "id_system_disp_titleid"))
			Mono::Set_Property(SettingElement, "Value", element, UI::DebugTitleIdLabel::ShowLabels ? Mono::New_String("1") : Mono::New_String("0"));
		else if (!strcmp(Id, "id_system_disp_debug_settings_panel"))
			Mono::Set_Property(SettingElement, "Value", element, UI::Custom_Content::Show_Debug_Settings ? Mono::New_String("1") : Mono::New_String("0"));
		else if (!strcmp(Id, "id_system_disp_app_home_panel"))
			Mono::Set_Property(SettingElement, "Value", element, UI::Custom_Content::Show_App_Home ? Mono::New_String("1") : Mono::New_String("0"));

	}
}

void ResetMenuItem(const char* Menu)
{
	MonoClass* UIManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "UIManager");
	Mono::Invoke<void>(Mono::App_exe, UIManager, Mono::Get_Instance(UIManager, "Instance"), "ResetMenuItem", Mono::New_String(Menu));
}

void AddMenuItem(MonoObject* ElementData)
{
	MonoClass* UIManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "UIManager");
	Mono::Invoke<void>(Mono::App_exe, UIManager, Mono::Get_Instance(UIManager, "Instance"), "AddMenuItem", ElementData, Mono::New_String(""));
}

MonoObject* NewElementData(const char* Id, const char* Title, const char* Title2, const char* Icon)
{
	MonoClass* ButtonElementData = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "ButtonElementData");
	MonoClass* ElementData = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "ElementData");
	MonoObject* Instance = Mono::New_Object(ButtonElementData);
	mono_runtime_object_init(Instance);

	Mono::Set_Property(ElementData, "Id", Instance, Mono::New_String(Id));
	Mono::Set_Property(ElementData, "Title", Instance, Mono::New_String(Title));
	Mono::Set_Property(ElementData, "SecondTitle", Instance, Mono::New_String(Title2));
	Mono::Set_Property(ElementData, "Icon", Instance, Mono::New_String(Icon));

	return Instance;
}

void Settings_Menu::OnPageActivating_Hook(MonoObject* Instance, MonoObject* page, MonoObject* e)
{
	if (Instance && page)
	{
		MonoClass* SettingPage = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingPage");
		char* Id = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingPage, "Id", page));

		klog("OnPageActivating: %s\n", Id);

		if (!strcmp(Id, "id_payloads"))
		{
			//Load payloads from Hdd
			//Or maybe from webapi?
			//Disable loading.
			AddMenuItem(NewElementData("id_Custom_Loader", "★Custom Payload", "Click here to start listening for payload on port 9020.", ""));
			AddMenuItem(NewElementData("id_GoldHEN_Loader", "GoldHEN", "New homebrew enabler developed by SiSTRo.", ""));
			AddMenuItem(NewElementData("id_HEN_Loader", "HEN", "Enables launching of homebrew apps.", ""));
			AddMenuItem(NewElementData("id_Updates_Loader", "Disable Updates", "Disables the consoles ability to download updates.", ""));
			AddMenuItem(NewElementData("id_Kernel_Loader", "Kernel Dumper", "Dumps your consoles Kernel from memory to a USB.", ""));
			ResetMenuItem("id_message");
		}
	}
}

void Settings_Menu::OnPress_Hook(MonoObject* Instance, MonoObject* element, MonoObject* e)
{
	if (Instance && element)
	{
		MonoClass* SettingElement = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingElement");
		MonoClass* ElementData = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "ElementData");
		char* Id = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingElement, "Id", element));
		char* Value = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingElement, "Value", element));

		if (!strcmp(Id, "id_Test"))
		{
			
			Notify("Test Button Pressed!");
		}
		else if (!strcmp(Id, "id_Test_2"))
			Notify("Test Button 2 Pressed!");
		else if (!strcmp(Id, "id_system_disp_devkit_panel"))
		{
			if (atoi(Value) > 0)
				UI::DevkitPanel::Show();
			else
				UI::DevkitPanel::Hide();
		}
		else if (!strcmp(Id, "id_system_disp_titleid"))
		{
			if (atoi(Value) > 0)
				UI::DebugTitleIdLabel::Show();
			else
				UI::DebugTitleIdLabel::Hide();
		}
		else if (!strcmp(Id, "id_system_disp_debug_settings_panel"))
		{
			UI::Custom_Content::Show_Debug_Settings = (atoi(Value) > 0);
			UI::Utilities::ReloadItemList();
		}
		else if (!strcmp(Id, "id_system_disp_app_home_panel"))
		{
			UI::Custom_Content::Show_App_Home = (atoi(Value) > 0);
			UI::Utilities::ReloadItemList();
		}
	}
}

void Settings_Menu::Log(char* fmt, ...)
{
	char va_Buffer[0x200];

	//Create full string from va list.
	va_list args;
	va_start(args, fmt);
	vsprintf(va_Buffer, fmt, args);
	va_end(args);

	char Message[0x200];
	sprintf(Message, "[Settings Menu] %s\n", va_Buffer);

	sceKernelDebugOutText(0, Message);
}

void Settings_Menu::Init()
{
	UI::DevkitPanel::Init();
	UI::DebugTitleIdLabel::Init();
	UI::Custom_Content::Init();

	//Detours
	Detour_GetManifestResourceStream = new Detour();
	Detour_OnCheckVisible = new Detour();
	Detour_OnPreCreate = new Detour();
	Detour_OnPageActivating = new Detour();
	Detour_OnPress = new Detour();

	//TODO: Update instruction counts and call stubs
	Detour_GetManifestResourceStream->DetourMethod(Mono::mscorlib, "System.Reflection", "Assembly", "GetManifestResourceStream", 1, (void*)GetManifestResourceStream_Hook, 17);
	Detour_OnCheckVisible->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnCheckVisible", 2, (void*)OnCheckVisible_Hook, 17);
	Detour_OnPreCreate->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPreCreate", 2, (void*)OnPreCreate_Hook, 17);
	Detour_OnPageActivating->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPageActivating", 2, (void*)OnPageActivating_Hook, 17);
	Detour_OnPress->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPress", 2, (void*)OnPress_Hook, 17);

	//Debug Settings Patch
	/*uint64_t IsDevKit_addr = Mono::Get_Address_of_Method(Mono::KernelSysWrapper, "Sce.Vsh", "KernelSysWrapper", "IsDevKit", 0);
	sceKernelMprotect((void*)IsDevKit_addr, 8, VM_PROT_ALL);
	memcpy((void*)IsDevKit_addr, "\x48\xc7\xc0\x01\x00\x00\x00\xC3", 8);

	uint64_t SblRcMgrIsAllowDebugMenuForSettings_addr = Mono::Get_Address_of_Method(Mono::platform_dll, "Sce.Vsh.ShellUI.Settings.Sbl", "SblWrapper", "SblRcMgrIsAllowDebugMenuForSettings", 0);
	sceKernelMprotect((void*)SblRcMgrIsAllowDebugMenuForSettings_addr, 8, VM_PROT_ALL);
	memcpy((void*)SblRcMgrIsAllowDebugMenuForSettings_addr, "\x48\xc7\xc0\x01\x00\x00\x00\xC3", 8);*/
}

void Settings_Menu::Term()
{
	UI::DevkitPanel::Term();
	UI::DebugTitleIdLabel::Term();
	UI::Custom_Content::Term();

	//Remove Denug Settings Patch

	//Clean up detours
	delete Detour_GetManifestResourceStream;
	delete Detour_OnCheckVisible;
	delete Detour_OnPreCreate;
	delete Detour_OnPageActivating;
	delete Detour_OnPress;
}