#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Factories/Factory.h"
#include "Help/MenuPathResolver.h"
#include "Styling/SlateStyle.h"
#include "QuickEditorService.generated.h"

UCLASS()
class UQuickEditorService : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UQuickEditorService();
	static UQuickEditorService& Get()
	{
		return *GEditor->GetEditorSubsystem<UQuickEditorService>();
	}

	void AddIcon(const FString& InName, const FString& InPath, FVector2D Size = FVector2D(40, 40));
	FString ResolveIconPath(const FString& InPath);

protected:
	// ~Begin USubsystem API 
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API
private:
	void _InitPaths();
	void _LoadIcons();
	void _InitPathIcons();
	void _ResolveIcon(FString IconPath, FString IconSize, UFunction* InFunc);
	void _ResolveThumbnail(FString IconPath, FString IconSize, UFunction* InFunc);
	
	void _InitLevelEditMenu();
	void _InitAssetEditMenu();
	void _InitActorEditMenu();
	void _InitToolBar();

	void _InitAssetNew();
	void _InitDetail();

	// TODO Detail

public:
	TMap<UClass*, UFunction*>	DetailCustomizationMap;
	TMap<UClass*, UFunction*>	PropertyCustomizationMap;
private:
	FLevelMenuPathResolver		LevelEditorPaths;
	FAssetMenuPathResolver		AssetEditorPaths;
	FActorMenuPathResolver		ActorEditorPaths;
	FToolBarPathResolver		ToolBarEditorPaths;
	TSharedPtr<FSlateStyleSet>	StyleSet;

	TArray<UClass*>				Factories;
};
