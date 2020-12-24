#pragma once
#include "CoreMinimal.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!   DO NOT USE THIS FILE IN YOUR PROJECT   !!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class FQECmdBuffer;

namespace QE
{
	// Actor actions 
	QUICKEDITOR_API extern TArray<AActor*>		SelectedActors;

	// Asset actions 
	QUICKEDITOR_API extern TArray<UObject*>		SelectedAssets;

	// Factory 
	QUICKEDITOR_API extern UClass*			NewObjectClass;
	QUICKEDITOR_API extern UObject*			NewObjectOuter;
	QUICKEDITOR_API extern FName			NewObjectName;
	QUICKEDITOR_API extern EObjectFlags		NewObjectFlag;
	QUICKEDITOR_API extern FString			ImportFileName;
	QUICKEDITOR_API extern UObject*			NewCreatedObject;
	QUICKEDITOR_API extern bool				bIsReimportAsset;

	// Window
	QUICKEDITOR_API extern int32					WindowId;
	QUICKEDITOR_API extern FString					WindowName;
	QUICKEDITOR_API extern TSharedPtr<SWidget>		WindowContent;
	QUICKEDITOR_API extern FVector2D				WindowSize;
	
	// Commands 
	QUICKEDITOR_API void CleanState();
	QUICKEDITOR_API void CleanCommands();
	QUICKEDITOR_API FQECmdBuffer& GetCmdBuffer();

	
	namespace Menu
	{
		QUICKEDITOR_API void MenuState(bool InEnable);
	}

	namespace Detail
	{
		QUICKEDITOR_API void DetailState(bool InEnable);
	}
	
	namespace Actor
	{
		QUICKEDITOR_API void ActorState(bool InEnable);
	}

	namespace Asset
	{
		QUICKEDITOR_API void AssetState(bool InEnable);
	}

	namespace AssetNew
	{
		QUICKEDITOR_API void AssetNewState(bool InEnable);
	}

	namespace Window
	{
		QUICKEDITOR_API void WindowState(bool InEnable);
	}
}