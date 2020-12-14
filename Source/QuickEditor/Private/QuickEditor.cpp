#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"

namespace QE
{
	// Global 
	bool bIsMenuState = false;
	bool bIsDetailState = false;
	bool bIsActorState = false;
	bool bIsAssetState = false;
	FQECmdBuffer	CmdBuffer;

	// Actor action  
	TArray<AActor*>		SelectedActors;

	// Asset action 
	TArray<UObject*>	SelectedAssets;

	// Factory
	UClass*			NewObjectClass;
	UObject*		NewObjectOuter;
	FName			NewObjectName;
	EObjectFlags	NewObjectFlag;
	FString			ImportFileName;
	
	
	void CleanState()
	{
		bIsMenuState = false;
		bIsDetailState = false;
		bIsActorState = false;
		bIsAssetState = false;
	}

	void CleanCommands()
	{
		CmdBuffer.Reset();
	}

	FQECmdBuffer& GetCmdBuffer()
	{
		return CmdBuffer;
	}

	void Asset::AssetState(bool InEnable)
	{
		bIsAssetState = InEnable;
	}

	void Actor::ActorState(bool InEnable)
	{
		bIsActorState = InEnable;
	}

	void Detail::DetailState(bool InEnable)
	{
		bIsDetailState = InEnable;
	}

	void Menu::MenuState(bool InEnable)
	{
		bIsMenuState = InEnable;
	}

	const TArray<AActor*>& Actor::GetSelectedActor()
	{
		return SelectedActors;
	}

	const TArray<UObject*>& Asset::GetSelectedAsset()
	{
		return SelectedAssets;
	}

	UClass* AssetNew::GetClass()
	{
		return NewObjectClass;
	}

	UObject* AssetNew::GetOuter()
	{
		return NewObjectOuter;
	}

	FName AssetNew::GetName()
	{
		return NewObjectName;
	}

	EObjectFlags AssetNew::GetFlags()
	{
		return NewObjectFlag;
	}

	FString AssetNew::GetImportFileName()
	{
		return ImportFileName;
	}
}

