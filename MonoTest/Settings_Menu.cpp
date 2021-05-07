#include "Common.h"
#include "Settings_Menu.h"

#include "UI.h"

//Embedded xmls
extern uint8_t settings_root[];
extern int32_t settings_root_Size;
extern uint8_t orbis_toolbox[];
extern int32_t orbis_toolbox_Size;
extern uint8_t external_hdd[];
extern int32_t external_hdd_Size;

//Detours
Detour* Settings_Menu::Detour_GetManifestResourceStream = nullptr;
Detour* Settings_Menu::Detour_OnCheckVisible = nullptr;
Detour* Settings_Menu::Detour_OnPreCreate = nullptr;
Detour* Settings_Menu::Detour_OnPageActivating = nullptr;
Detour* Settings_Menu::Detour_OnPress = nullptr;

//Patches
Patcher* Settings_Menu::Patch_IsDevkit;
Patcher* Settings_Menu::Patch_AllowDebugMenu;

/*
	GetManifestResourceStream:
		This is the method I hook that loads internal packed resources 
		from the mono UI. I intercept the uri and force it to return a 
		new memory stream of the bytes for our custom xml.
*/

uint64_t Settings_Menu::GetManifestResourceStream_Hook(uint64_t inst, MonoString* FileName)
{
	char* str = mono_string_to_utf8(FileName);
	klog("****\nFileName: %s\n****\n", str);

	if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.SettingsRoot.data.settings_root.xml"))
		return (uint64_t)New_MemoryStream(settings_root, settings_root_Size);
	else if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.orbis_toolbox.xml"))
		return (uint64_t)New_MemoryStream(orbis_toolbox, orbis_toolbox_Size);
	else if (!strcmp(str, "Sce.Vsh.ShellUI.src.Sce.Vsh.ShellUI.Settings.Plugins.external_hdd.xml"))
		return (uint64_t)New_MemoryStream(external_hdd, external_hdd_Size);
	else
		return Detour_GetManifestResourceStream->Stub<uint64_t>(inst, FileName);
}

/*
	Settings Root Hooks:
		I set my custom menu as the plugin of the root settings page
		this allows me to hook all the call backs from inside this
		page and run my own code.

		Note: in the xml "plugin="settings_root_plugin""
*/

/*
	OnCheckVisible:
		This could allow us to show and hide elements on each page
		much like the id_message which is a loading symbol.
*/

void Settings_Menu::OnCheckVisible_Hook(MonoObject* Instance, MonoObject* element, MonoObject* e)
{
	if (Instance && element)
	{
		MonoClass* SettingElement = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.Core", "SettingElement");
		char* Id = mono_string_to_utf8(Mono::Get_Property<MonoString*>(SettingElement, "Id", element));
		if (!strcmp(Id, "id_message"))
			Mono::Set_Property(SettingElement, "Visible", element, false);
	}
	Detour_OnCheckVisible->Stub<void>(Instance, element, e);
}

/*
	OnPreCreate:
		This Hook allows us to set values of each menu element
		depending on what is set prior. For example I use this
		to set the check boxes to match the previously selected
		values.
*/

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
		else if (!strcmp(Id, "id_orbislib"))
			Mono::Set_Property(SettingElement, "Value", element, Mono::New_String("Stopped"));
		else if(!strcmp(Id, "id_orbisftp"))
			Mono::Set_Property(SettingElement, "Value", element, Mono::New_String("Running"));
		
	}
	Detour_OnPreCreate->Stub<void>(Instance, element, e);
}

/*
	OnPageActivating:
		Hooking this allows us to add custom elements when a
		page is loading like for example how I will be using
		it to parse the payloads from the HDD and display
		them to be loaded.
*/

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
	Detour_OnPageActivating->Stub<void>(Instance, page, e);
}

/*
	OnPress:
		This Hook allows us to catch when a element is selected
		or its value is changed.
*/

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

	Detour_OnPress->Stub<void>(Instance, element, e);
}

void Settings_Menu::Log(const char* fmt, ...)
{
	char va_Buffer[0x200];

	//Create full string from va list.
	va_list args;
	va_start(args, fmt);
	vsprintf(va_Buffer, fmt, args);
	va_end(args);

	klog("[Settings Menu] %s\n", va_Buffer);
}

void Settings_Menu::Init()
{
	Log("Init");

	UI::DevkitPanel::Init();
	UI::DebugTitleIdLabel::Init();
	UI::Custom_Content::Init();

	//Detours
	Log("Init Detours");
	Detour_GetManifestResourceStream = new Detour();
	Detour_OnCheckVisible = new Detour();
	Detour_OnPreCreate = new Detour();
	Detour_OnPageActivating = new Detour();
	Detour_OnPress = new Detour();

	Log("Detour Methods");
	Detour_GetManifestResourceStream->DetourMethod(Mono::mscorlib, "System.Reflection", "Assembly", "GetManifestResourceStream", 1, (void*)GetManifestResourceStream_Hook, 17);
	Detour_OnCheckVisible->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnCheckVisible", 2, (void*)OnCheckVisible_Hook, 16);
	Detour_OnPreCreate->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPreCreate", 2, (void*)OnPreCreate_Hook, 14);
	Detour_OnPageActivating->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPageActivating", 2, (void*)OnPageActivating_Hook, 14);
	Detour_OnPress->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Settings.SettingsRoot", "SettingsRootHandler", "OnPress", 2, (void*)OnPress_Hook, 15);

	//Debug Settings Patch
	Patch_IsDevkit = new Patcher();
	Patch_AllowDebugMenu = new Patcher();

	Log("Install Patches");
	Patch_IsDevkit->Install_Method_Patch(Mono::KernelSysWrapper, "Sce.Vsh", "KernelSysWrapper", "IsDevKit", 0, 0, "\x48\xc7\xc0\x01\x00\x00\x00\xC3", 8);
	Patch_AllowDebugMenu->Install_Method_Patch(Mono::platform_dll, "Sce.Vsh.ShellUI.Settings.Sbl", "SblWrapper", "SblRcMgrIsAllowDebugMenuForSettings", 0, 0, "\x48\xc7\xc0\x01\x00\x00\x00\xC3", 8);

	Log("Init Complete");
}

void Settings_Menu::Term()
{
	UI::DevkitPanel::Term();
	UI::DebugTitleIdLabel::Term();
	UI::Custom_Content::Term();

	//Remove Denug Settings Patch
	delete Patch_IsDevkit;
	delete Patch_AllowDebugMenu;

	//Clean up detours
	delete Detour_GetManifestResourceStream;
	delete Detour_OnCheckVisible;
	delete Detour_OnPreCreate;
	delete Detour_OnPageActivating;
	delete Detour_OnPress;
}