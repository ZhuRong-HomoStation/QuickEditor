#pragma once
#include "CoreMinimal.h"
 
namespace QE
{
	namespace Menu
	{
		QUICKEDITOR_API void AddEntry(const FString& InEntryName, const FString& InEntryIcon, const FSimpleDelegate& InEntryEvent);
		QUICKEDITOR_API void AddWidget(TSharedPtr<SWidget> InWidget);
		QUICKEDITOR_API void BeginSection(const FString& InSecName);
		QUICKEDITOR_API void EndSection();
	}

	namespace Detail
	{
		QUICKEDITOR_API const TArray<UObject*>& GetSelectedObjects();
		QUICKEDITOR_API void HideParent(const FString& InName);
		QUICKEDITOR_API void ShowParent(const FString& InName);
		QUICKEDITOR_API void HideCategory(const FString& InCategory);
		QUICKEDITOR_API void EditCategory(const FString& InCategory, int32 InPriority /*0-4, see also ECategoryPriority*/);
		QUICKEDITOR_API void HideProperty(const FString& InPropertyName);
		QUICKEDITOR_API void EditProperty(const FString& InPropertyName, const FString& InOverrideName, TSharedPtr<SWidget> InWidget);
		QUICKEDITOR_API void EditProperty(const FString& InPropertyName, TSharedPtr<SWidget> InNameWidget, TSharedPtr<SWidget> InValueWidget);
		QUICKEDITOR_API void AddWidget(const FString& InName, const FString& InSearchName, TSharedPtr<SWidget> InWidget);
		QUICKEDITOR_API void AddWidget(const FString& InSearchName, TSharedPtr<SWidget> InNameWidget, TSharedPtr<SWidget> InValueWidget);
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
		QUICKEDITOR_API UClass*			GetClass();
		QUICKEDITOR_API UObject*		GetOuter();
		QUICKEDITOR_API FName			GetName();
		QUICKEDITOR_API EObjectFlags	GetFlags();
		QUICKEDITOR_API FString			GetImportFilePath();
		QUICKEDITOR_API void			CreatedObject(UObject* InObject);
		QUICKEDITOR_API UObject*		CreatedObject();
		QUICKEDITOR_API UObject*		CreateDefault();
		QUICKEDITOR_API bool			IsReimport();
	}

	namespace Window
	{
		QUICKEDITOR_API int32	GetID();
		QUICKEDITOR_API void	WndName(const FString& InName);
		QUICKEDITOR_API void	WndContent(TSharedRef<SWidget> InWidget);
		QUICKEDITOR_API void	WndSize(FVector2D Size);
	}
}

