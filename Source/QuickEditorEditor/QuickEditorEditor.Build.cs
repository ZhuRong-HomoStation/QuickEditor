using System.IO;
using UnrealBuildTool;

public class QuickEditorEditor : ModuleRules
{
	public QuickEditorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core" ,
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"QuickEditor" ,
				"LevelEditor",
				"EditorSubsystem",
				"ToolMenus" ,
				"MainFrame" ,
				"ContentBrowser" ,
				"Projects" ,
				"UnrealEd" ,
			});
	}
}
