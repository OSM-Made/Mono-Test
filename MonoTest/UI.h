#pragma once

class UI
{
public:
	class Custom_Content
	{
	private:
		static Detour* Detour_ExecuteSelectQuery;
		static Detour* Detour_ExecuteCountQuery;
		static Detour* Detour_StartDebugSettings;
		static Detour* Detour_GetIconPath;

		static MonoObject* ExecuteSelectQuery_Hook(MonoObject* Instance, int offset, int limit);
		static int ExecuteCountQuery_Hook(MonoObject* Instance);
		static void StartDebugSettings_Hook(MonoObject* Instance);
		static MonoString* GetIconPath_Hook(MonoObject* item, bool withTheme);

	public:
		static bool Show_App_Home;
		static bool Show_Debug_Settings;

		static void Init();
		static void Term();
	};

	class DebugTitleIdLabel
	{
	private:
		static void(*CreateDebugTitleIdLabel)(MonoObject* Instance);
		static Detour* Detour_ContentDecoratorBase_Constructor;
		static uint64_t ContentDecoratorBase_Constructor_Hook(MonoObject* Instance, uint64_t param);

		static Patcher* Patch_createDevKitPanel;

		static void AddTitleId(MonoObject* m_contentsGridList);
		static void RemoveTitleId(MonoObject* m_contentsGridList);

	public:
		static bool ShowLabels;

		static void Show();
		static void Hide();
		static void Init();
		static void Term();
	};

	class DevkitPanel
	{
	private:
		static void(*createDevKitPanel)(MonoObject* Instance);
		static Detour* Detour_AreaManager_Constructor;
		static uint64_t AreaManager_Constructor_Hook(MonoObject* Instance);

	public:
		static bool ShowPanel;

		static void Show();
		static void Hide();
		static bool GetState();
		static void Init();
		static void Term();
	};

	class Drawing
	{
	public:
		enum RenderingOrder
		{
			First = -1,
			DontCare,
			Last
		};

		enum Orientation
		{
			Horizontal,
			Vertical
		};

		enum VerticalAlignment
		{
			vTop,
			vBottom,
			vCenter
		};

		enum HorizontalAlignment
		{
			hLeft,
			hCenter,
			hRight
		};

		enum FontStyle
		{
			fsNormal,
			fsItalic = 2U
		};

		enum FontWeight
		{
			//[Obsolete("This value is not used for Orbis. Use 'Light' or 'Medium'.")]
			fwNormal,
			//[Obsolete("Use 'Medium' instead of Bold.")]
			fwBold,
			fwLight,
			fwMedium,
			//[Obsolete("This value is for GLS. Use 'Medium' instead of LegacyBold.")]
			fwLegacyBold = 1000U
		};

		static MonoObject* Get_Top_Scene();
		static MonoObject* Get_root_Widget();
		static MonoObject* Adjust_Content(int AlignOrientation, float PaddingLeft, float PaddingRight, float PaddingTop, float PaddingBottom);
		static MonoObject* Fit_To_Children();
		static void Append_Child(MonoObject* Parent, MonoObject* Child);
		static void Remove_Child(MonoObject* Parent, const char* Child);
		static MonoObject* Panel(const char* Name, float X, float Y, float Width, float Height, float R, float G, float B, float A, RenderingOrder RenderingOrder);
		static MonoObject* IUFont(int size, int style, int weight);
		static MonoObject* Label(const char* Name, const char* Text, int Font_Size, FontStyle Style, FontWeight Weight, float X, float Y, float R, float G, float B, float A);
		 
		static void Stop_Drawing_Element(const char* Name);


		static void Init();
		static void Term();

	private:
		std::map<const char*, MonoObject*> Widgets;
	};


	class Utilities
	{
	public:
		static void SetVersionString(const char* str);
		static void ReloadItemList();
		static MonoObject* NewAppBrowseItem(const char* TitleId, const char* TitleName);
		static MonoObject* Get_Top_Scene();
		static MonoObject* Get_root_Widget();

	private:

	};

private:

};