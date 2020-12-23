using UnrealBuildTool;

public class QuickEditor : ModuleRules
{
	public QuickEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			});

		// editor case 
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"LevelEditor",
					"EditorSubsystem",
					"ToolMenus" ,
					"MainFrame" ,
					"ContentBrowser" ,
					"Projects" ,
					"UnrealEd" ,
					"PropertyEditor" ,
				});
		}
	}
}
