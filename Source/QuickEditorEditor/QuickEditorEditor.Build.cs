using System.IO;
using Tools.DotNETCommon;
using UnrealBuildTool;

public class QuickEditorEditor : ModuleRules
{
	public QuickEditorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../QuickEditor/Private"));
		
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
				"PropertyEditor" ,
				"EditorStyle" ,
			});
	}
}
