#pragma once
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "QuickEditorConfig.generated.h"

UCLASS(Config=QuickEditor, DefaultConfig)
class UQuickEditorConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UQuickEditorConfig();

	static UQuickEditorConfig* Get()
	{
		static UQuickEditorConfig* Config = nullptr;
		if (!Config)
		{
			Config = NewObject<UQuickEditorConfig>();
			Config->LoadConfig();
			Config->AddToRoot();
		}
		return Config;
	}

	// main menu path icons
	UPROPERTY(EditAnywhere, Config)
	TMap<FString, FString>		MainMenuPathIcons;

	// actor action path icons
	UPROPERTY(EditAnywhere, Config)
	TMap<FString, FString>		ActorActionPathIcons;

	// asset action path icons
	UPROPERTY(EditAnywhere, Config)
	TMap<FString, FString>		AssetActionPathIcons;

	// tool bar path icons
	UPROPERTY(EditAnywhere, Config)
	TMap<FString, FString>		ToolBarPathIcons;
};
