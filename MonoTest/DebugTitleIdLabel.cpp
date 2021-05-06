#include "Common.h"
#include "UI.h"

void(*UI::DebugTitleIdLabel::CreateDebugTitleIdLabel)(MonoObject* Instance);
Detour* UI::DebugTitleIdLabel::Detour_ContentDecoratorBase_Constructor;

bool UI::DebugTitleIdLabel::ShowLabels = false;

uint64_t UI::DebugTitleIdLabel::ContentDecoratorBase_Constructor_Hook(MonoObject* Instance, uint64_t param)
{
	uint64_t res = Detour_ContentDecoratorBase_Constructor->Stub<uint64_t>(Instance, param);

	if (ShowLabels)
		CreateDebugTitleIdLabel(Instance);

	return res;
}

void UI::DebugTitleIdLabel::AddTitleId(MonoObject* m_contentsGridList)
{
	MonoClass* ReadOnlyCollection = Mono::Get_Class(Mono::mscorlib, "System.Collections.ObjectModel", "ReadOnlyCollection`1");

	if (m_contentsGridList)
	{
		MonoObject* ActiveItems = Mono::Get_Property<MonoObject*>(Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "ListPanelBase"), "ActiveItems", m_contentsGridList);

		for (int i = 0; i < Mono::Get_Property<int>(ReadOnlyCollection, "Count", ActiveItems); i++)
		{
			MonoObject* Member = Mono::Invoke<MonoObject*>(Mono::mscorlib, ReadOnlyCollection, ActiveItems, "get_Item", i);
			MonoObject* ListVisualizer = Mono::Get_Property<MonoObject*>(Mono::Get_Class(Mono::Vsh_Lx, "Sce.Vsh.Lx", "ListItem"), "ListVisualizer", Member);
			MonoObject* m_decorator = Mono::Get_Field<MonoObject*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Library", "ContentVisualizer"), ListVisualizer, "m_decorator");
			CreateDebugTitleIdLabel(m_decorator);
		}
	}
}

void UI::DebugTitleIdLabel::RemoveTitleId(MonoObject* m_contentsGridList)
{
	MonoClass* ReadOnlyCollection = Mono::Get_Class(Mono::mscorlib, "System.Collections.ObjectModel", "ReadOnlyCollection`1");

	if (m_contentsGridList)
	{
		MonoObject* ActiveItems = Mono::Get_Property<MonoObject*>(Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "ListPanelBase"), "ActiveItems", m_contentsGridList);

		for (int i = 0; i < Mono::Get_Property<int>(ReadOnlyCollection, "Count", ActiveItems); i++)
		{
			MonoObject* Member = Mono::Invoke<MonoObject*>(Mono::mscorlib, ReadOnlyCollection, ActiveItems, "get_Item", i);
			MonoObject* ListVisualizer = Mono::Get_Property<MonoObject*>(Mono::Get_Class(Mono::Vsh_Lx, "Sce.Vsh.Lx", "ListItem"), "ListVisualizer", Member);
			MonoObject* m_decorator = Mono::Get_Field<MonoObject*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Library", "ContentVisualizer"), ListVisualizer, "m_decorator");
			MonoObject* m_iconImageBox = Mono::Get_Field<MonoObject*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.Library", "ContentDecoratorBase"), m_decorator, "m_iconImageBox");

			if (m_iconImageBox)
			{
				MonoArray* Children = Mono::Invoke<MonoArray*>(Mono::App_exe, Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Node`1"), m_iconImageBox, "GetChildrenArray");

				for (int i = 0; i < mono_array_length(Children); i++)
				{
					MonoObject* Instance = mono_array_get(Children, MonoObject*, i);

					if (!Instance)
						continue;

					if (!Instance->vtable)
						continue;

					if (!Instance->vtable->klass)
						continue;

					if (strcmp(Instance->vtable->klass->name, "Label"))
						continue;

					Mono::Invoke<void>(Mono::App_exe, Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Widget"), Instance, "RemoveFromParent");
				}
			}
		}
	}
}

void UI::DebugTitleIdLabel::Show()
{
	MonoClass* ContentsAreaManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentsAreaManager");
	MonoObject* m_scene = Mono::Get_Field<MonoObject*>(ContentsAreaManager, Mono::Get_Instance(ContentsAreaManager, "Instance"), "m_scene");
	MonoArray* m_contentsGridList = Mono::Get_Field<MonoArray*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentAreaScene"), m_scene, "m_contentsGridList");

	AddTitleId(mono_array_get(m_contentsGridList, MonoObject*, 0));
	AddTitleId(mono_array_get(m_contentsGridList, MonoObject*, 1));

	ShowLabels = true;
}

void UI::DebugTitleIdLabel::Hide()
{
	MonoClass* ContentsAreaManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentsAreaManager");
	MonoObject* m_scene = Mono::Get_Field<MonoObject*>(ContentsAreaManager, Mono::Get_Instance(ContentsAreaManager, "Instance"), "m_scene");
	MonoArray* m_contentsGridList = Mono::Get_Field<MonoArray*>(Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "ContentAreaScene"), m_scene, "m_contentsGridList");

	RemoveTitleId(mono_array_get(m_contentsGridList, MonoObject*, 0));
	RemoveTitleId(mono_array_get(m_contentsGridList, MonoObject*, 1));

	ShowLabels = false;
}

void UI::DebugTitleIdLabel::Init()
{
	uint64_t CreateDebugTitleIdLabel_addr = Mono::Get_Address_of_Method(Mono::App_exe, "Sce.Vsh.ShellUI.Library", "ContentDecoratorBase", "CreateDebugTitleIdLabel", 0);
	CreateDebugTitleIdLabel = decltype(CreateDebugTitleIdLabel)(CreateDebugTitleIdLabel_addr);

	//Patch RegMgr Check 
	sceKernelMprotect((void*)(CreateDebugTitleIdLabel_addr + 0x2C), 0x20, VM_PROT_ALL);
	memcpy((void*)(CreateDebugTitleIdLabel_addr + 0x2C), "\x90\x90\x90\x90\x90\x90", 6);

	Detour_ContentDecoratorBase_Constructor = new Detour();
	Detour_ContentDecoratorBase_Constructor->DetourMethod(Mono::App_exe, "Sce.Vsh.ShellUI.Library", "ContentDecoratorBase", ".ctor", 1, (void*)ContentDecoratorBase_Constructor_Hook, 15);
}

void UI::DebugTitleIdLabel::Term()
{
	delete Detour_ContentDecoratorBase_Constructor;
}