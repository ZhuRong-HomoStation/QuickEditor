#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"

namespace QE
{
	bool bIsMenuState = false;
	bool bIsDetailState = false;
	bool bIsActorState = false;
	bool bIsAssetState = false;
	FQECmdBuffer	CmdBuffer;

	TArray<AActor*>		SelectedActors;
	TArray<UObject*>	SelectedAssets;
	
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
}

