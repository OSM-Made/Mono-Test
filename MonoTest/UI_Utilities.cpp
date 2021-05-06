#include "Common.h"
#include "UI.h"

void UI::Utilities::SetVersionString(const char* str)
{
	struct MonoClass* SystemSoftwareVersionInfo = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.AppSystem", "SystemSoftwareVersionInfo");
	Mono::Set_Property(SystemSoftwareVersionInfo, "DisplayVersion", Mono::Get_Instance(SystemSoftwareVersionInfo, "Instance"), Mono::New_String(str));
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
	MonoClass* AppBrowseItem = Mono::Get_Class(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItem");
	MonoClass* AppBrowseItemBase = Mono::Get_Class(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItemBase");
	MonoClass* AppBrowseItemLite = Mono::Get_Class(Mono::Accessor_Db, "Sce.Vsh.Accessor.Db", "AppBrowseItemLite");

	MonoObject* Instance = Mono::New_Object(AppBrowseItem);
	mono_runtime_object_init(Instance);

	Mono::Set_Property(AppBrowseItemLite, "TitleId", Instance, Mono::New_String(TitleId));
	Mono::Set_Property(AppBrowseItemBase, "TitleName", Instance, Mono::New_String(TitleName));
	Mono::Set_Property(AppBrowseItemBase, "MetaDataPath", Instance, 0);

	return Instance;
}