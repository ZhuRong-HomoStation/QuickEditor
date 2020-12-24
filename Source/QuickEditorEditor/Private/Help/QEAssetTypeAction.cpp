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
