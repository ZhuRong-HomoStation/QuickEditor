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

protected:
	// ~Begin USubsystem API 
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API
private:
	void _InitPaths();
	void _LoadIcons();
	
	void _InitLevelEditMenu();
	void _InitAssetEditMenu();
	void _InitActorEditMenu();
	void _InitToolBar();

	void _InitAssetNew();
	void _InitDetail();

	// TODO Execute menu commands
	// TODO CreateAsset
	// TODO Detail

private:
	FLevelMenuPathResolver		LevelEditorPaths;
	FAssetMenuPathResolver		AssetEditorPaths;
	FActorMenuPathResolver		ActorEditorPaths;
	FToolBarPathResolver		ToolBarEditorPaths;
	TSharedPtr<FSlateStyleSet>	StyleSet;

	TArray<UClass*>				Factories;
};
