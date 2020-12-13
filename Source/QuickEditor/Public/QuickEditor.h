#pragma once
#include "CoreMinimal.h"
 
namespace QE
{
	namespace Menu
	{
		QUICKEDITOR_API void AddEntry(const FString& InEntryName, const FString& InEntryIcon, const FSimpleDelegate& InDelegate);
		QUICKEDITOR_API void AddWidget(TSharedPtr<SWidget> InWidget);
		QUICKEDITOR_API void BeginSection(const FString& InSecName);
		QUICKEDITOR_API void EndSection();
	}

	namespace Detail
	{
		QUICKEDITOR_API void HideParent(const FString& InName);
		QUICKEDITOR_API void ShowParent(const FString& InName);
		QUICKEDITOR_API void HideCategory(const FString& InName);
	}
	
	namespace Actor
	{
		QUICKEDITOR_API const TArray<AActor*>& GetSelectedActor();
	}

	namespace Asset
	{
		QUICKEDITOR_API const TArray<UObject*>& GetSelectedAsset();
	}

	namespace AssetNew
	{
		
	}
}

