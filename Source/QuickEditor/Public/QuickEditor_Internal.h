#pragma once
#include "CoreMinimal.h"

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
}