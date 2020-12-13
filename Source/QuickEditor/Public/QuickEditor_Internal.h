#pragma once
#include "CoreMinimal.h"

class FQECmdBuffer;

namespace QE
{
	QUICKEDITOR_API extern TArray<AActor*>		SelectedActors;
	QUICKEDITOR_API extern TArray<UObject*>		SelectedAssets;
	
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