#include "QEAssetTypeAction.h"
#include "Config/QuickEditorConfig.h"

void FQEAssetTypeAction::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
													TArray<FString>& OutSourceFilePaths) const
{
	for (UObject* Asset : TypeAssets)
	{
		FString* Path = UQuickEditorConfig::Get()->ImportFilePaths.Find(Asset->GetPathName());
		if (Path)
		{
			OutSourceFilePaths.Add(*Path);
		}
	}
}

void FQEAssetTypeAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	Super::OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

void FQEAssetTypeAction::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

bool FQEAssetTypeAction::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}
