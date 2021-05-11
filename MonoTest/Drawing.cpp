#include "Common.h"
#include "UI.h"

MonoObject* UI::Drawing::Get_Top_Scene()
{
	MonoClass* AreaManager = Mono::Get_Class(Mono::App_exe, "Sce.Vsh.ShellUI.TopMenu", "AreaManager");
	return Mono::Invoke<MonoObject*>(Mono::App_exe, AreaManager, Mono::Get_Instance(AreaManager, "Instance"), "GetTopScene");
}

MonoObject* UI::Drawing::Get_root_Widget()
{
	return Mono::Get_Property<MonoObject*>(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Scene", Get_Top_Scene(), "RootWidget");
}

MonoObject* UI::Drawing::Adjust_Content(int AlignOrientation, float PaddingLeft, float PaddingRight, float PaddingTop, float PaddingBottom)
{
	MonoClass* AdjustContent = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "AdjustContent");

	//Allocates memory for our new instance of a class.
	MonoObject* AdjustContent_Instance = Mono::New_Object(AdjustContent);

	//Call the default no param constructor.
	mono_runtime_object_init(AdjustContent_Instance);

	//Add Properties.
	Mono::Set_Property(AdjustContent, AdjustContent_Instance, "AlignOrientation", AlignOrientation);
	Mono::Set_Property(AdjustContent, AdjustContent_Instance, "PaddingLeft", PaddingLeft);
	Mono::Set_Property(AdjustContent, AdjustContent_Instance, "PaddingRight", PaddingRight);
	Mono::Set_Property(AdjustContent, AdjustContent_Instance, "PaddingTop", PaddingTop);
	Mono::Set_Property(AdjustContent, AdjustContent_Instance, "PaddingBottom", PaddingBottom);

	return AdjustContent_Instance;
}

MonoObject* UI::Drawing::Fit_To_Children()
{
	MonoClass* FitToChildren = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "FitToChildren");

	MonoObject* FitToChildren_Instance = Mono::New_Object(FitToChildren);
	mono_runtime_object_init(FitToChildren_Instance);

	return FitToChildren_Instance;
}

void UI::Drawing::Append_Child(MonoObject* Parent, MonoObject* Child)
{
	MonoClass* Widget = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Widget");
	Mono::Invoke<void>(Mono::App_exe, Widget, Parent, "AppendChild", Child);
}

void UI::Drawing::Remove_Child(MonoObject* Parent, const char* Child)
{
	MonoClass* Widget = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Widget");

	MonoObject* ChildWidget = Mono::Invoke<MonoObject*>(Mono::App_exe, Widget, Parent, "FindWidgetByName", Mono::New_String(Child));
	Mono::Invoke<void>(Mono::App_exe, Widget, ChildWidget, "RemoveFromParent");
}

MonoObject* UI::Drawing::Panel(const char* Name, float X, float Y, float Width, float Height, float R, float G, float B, float A, RenderingOrder RenderingOrder)
{
	MonoClass* Widget = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Widget");
	MonoClass* Panel = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Panel");
	MonoClass* FontConfig = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "FontConfig");

	MonoObject* rootWidget = Get_root_Widget();
	Mono::Set_Property(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Widget", rootWidget, "ClipChildren", false);

	//Allocates memory for our new instance of a class.
	MonoObject* PanelInstance = Mono::New_Object(Panel);

	//Call Constructor.
	mono_runtime_object_init(PanelInstance);

	Mono::Set_Property(Panel, PanelInstance, "Name", Mono::New_String(Name));
	Mono::Set_Property(Panel, PanelInstance, "X", X);
	Mono::Set_Property(Panel, PanelInstance, "Y", Y);
	Mono::Set_Property(Panel, PanelInstance, "Width", Width);
	Mono::Set_Property(Panel, PanelInstance, "Height", Height);
	Mono::Set_Property_Invoke(Panel, PanelInstance, "BackgroundColor", NewUIColor(R, G, B, A));
	Mono::Set_Property(Panel, PanelInstance, "RenderingOrder", RenderingOrder);
	Mono::Set_Property(Panel, PanelInstance, "LayoutRule", Adjust_Content(Orientation::Vertical, 4, 4, 4, 4));
	//Mono::Set_Property(Panel, PanelInstance, "LayoutRule", Fit_To_Children());

	MonoObject* FontConfig_Instance = Mono::Get_Property<MonoObject*>(Widget, PanelInstance, "FontConfig");
	Mono::Set_Property(FontConfig, FontConfig_Instance, "BoldFontEnabled", false);
	Mono::Set_Property(FontConfig, FontConfig_Instance, "LargeFontEnabled", false);

	Append_Child(rootWidget, PanelInstance);

	return PanelInstance;
}

MonoObject* UI::Drawing::IUFont(int size, int style, int weight)
{
	MonoClass* UIFont = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "UIFont");

	//Allocates memory for our new instance of a class.
	MonoObject* UIFont_Instance = Mono::New_Object(UIFont);
	Mono::Invoke<void>(Mono::App_exe, UIFont, (MonoObject*)mono_object_unbox(UIFont_Instance), ".ctor", size, style, weight);

	return (MonoObject*)mono_object_unbox(UIFont_Instance);
}

MonoObject* UI::Drawing::Label(const char* Name, const char* Text, int Font_Size, FontStyle Style, FontWeight Weight, float X, float Y, float R, float G, float B, float A)
{
	MonoClass* Label = Mono::Get_Class(Mono::Highlevel_UI2, "Sce.PlayStation.HighLevel.UI2", "Label");

	//Allocates memory for our new instance of a class.
	MonoObject* Label_Instance = Mono::New_Object(Label);

	//Call the default no param constructor.
	mono_runtime_object_init(Label_Instance);

	Mono::Set_Property(Label, Label_Instance, "Name", Mono::New_String(Name));
	Mono::Set_Property(Label, Label_Instance, "Text", Mono::New_String(Text));
	Mono::Set_Property(Label, Label_Instance, "VerticalAlignment", VerticalAlignment::vCenter);
	Mono::Set_Property(Label, Label_Instance, "HorizontalAlignment", HorizontalAlignment::hCenter);
	Mono::Set_Property_Invoke(Label, Label_Instance, "TextColor", NewUIColor(R, G, B, A));
	Mono::Set_Property_Invoke(Label, Label_Instance, "Font", IUFont(Font_Size, Style, Weight));
	Mono::Set_Property(Label, Label_Instance, "X", X);
	Mono::Set_Property(Label, Label_Instance, "Y", Y);
	Mono::Set_Property(Label, Label_Instance, "FitWidthToText", true);
	Mono::Set_Property(Label, Label_Instance, "FitHeightToText", true);

	return Label_Instance;
}

void UI::Drawing::Init()
{

}

void UI::Drawing::Term()
{

	//Remove all of the Widgets we are drawing.
}