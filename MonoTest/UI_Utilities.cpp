#include "Common.h"
#include "UI.h"

void UI::Utilities::SetVersionString(const char* str)
{
	MonoClass* SystemSoftwareVersionInfo = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.AppSystem", "SystemSoftwareVersionInfo");
	Mono::Set_Property(SystemSoftwareVersionInfo, Mono::Get_Instance(SystemSoftwareVersionInfo, "Instance"), "DisplayVersion", Mono::New_String(str));
}

void UI::Utilities::ReloadItemList()
{
	MonoClass* ContentsAreaManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentsAreaManager");
	MonoClass* ContentsList = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentsList");

	MonoObject* m_scene = Mono::Get_Field<MonoObject*>(ContentsAreaManager, Mono::Get_Instance(ContentsAreaManager, "Instance"), "m_scene");
	MonoArray* m_contentsList = Mono::Get_Field<MonoArray*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentAreaScene"), m_scene, "m_contentsList");
	MonoObject* m_contentsList_0 = mono_array_get(m_contentsList, MonoObject*, 0);

	if (m_contentsList_0)
		Mono::Invoke<void>(Mono::App_exe, ContentsList, m_contentsList_0, "ReloadItemSource");
}

MonoObject* UI::Utilities::NewAppBrowseItem(const char* TitleId, const char* TitleName)
{
	MonoObject* Instance = Mono::New_Object(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItem");
	mono_runtime_object_init(Instance);

	Mono::Set_Property(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItemLite", Instance, "TitleId", Mono::New_String(TitleId));
	Mono::Set_Property(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItemBase", Instance, "TitleName", Mono::New_String(TitleName));
	Mono::Set_Property(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItemBase", Instance, "MetaDataPath", 0);

	return Instance;
}

MonoObject* UI::Utilities::Get_Top_Scene()
{
	MonoClass* AreaManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "AreaManager");
	return Mono::Invoke<MonoObject*>(Mono::App_exe, AreaManager, Mono::Get_Instance(AreaManager, "Instance"), "GetTopScene");
}

MonoObject*UI::Utilities::Get_root_Widget()
{
	return Mono::Get_Property<MonoObject*>(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Scene", Get_Top_Scene(), "RootWidget");
}