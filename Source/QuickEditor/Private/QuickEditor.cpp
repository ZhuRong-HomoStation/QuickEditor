#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"

#define CHECK_STATE_MENU		checkf(bIsMenuState, TEXT("function in QE::Menu can only called on MenuState"));
#define CHECK_STATE_DETAIL		checkf(bIsDetailState, TEXT("function in QE::Detail can only called on DetailState"));
#define CHECK_STATE_ACTOR		checkf(bIsActorState, TEXT("function in QE::Actor can only called on ActorState"));
#define CHECK_STATE_ASSET		checkf(bIsAssetState, TEXT("function in QE::Asset can only called on AssetState"));
#define CHECK_STATE_ASSET_NEW	checkf(bIsAssetNewState, TEXT("function in QE::AssetNew can only called on AssetNewState"));

namespace QE
{
	// Global 
	bool bIsMenuState = false;
	bool bIsDetailState = false;
	bool bIsActorState = false;
	bool bIsAssetState = false;
	bool bIsAssetNewState = false;
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

	void AssetNew::AssetNewState(bool InEnable)
	{
		bIsAssetNewState = InEnable;
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
		CHECK_STATE_ACTOR;
		return SelectedActors;
	}

	const TArray<UObject*>& Asset::GetSelectedAsset()
	{
		CHECK_STATE_ASSET;
		return SelectedAssets;
	}

	UClass* AssetNew::GetClass()
	{
		CHECK_STATE_ASSET_NEW;
		return NewObjectClass;
	}

	UObject* AssetNew::GetOuter()
	{
		CHECK_STATE_ASSET_NEW;
		return NewObjectOuter;
	}

	FName AssetNew::GetName()
	{
		CHECK_STATE_ASSET_NEW;
		return NewObjectName;
	}

	EObjectFlags AssetNew::GetFlags()
	{
		CHECK_STATE_ASSET_NEW;
		return NewObjectFlag;
	}

	FString AssetNew::GetImportFileName()
	{
		CHECK_STATE_ASSET_NEW;
		return ImportFileName;
	}

	void Menu::AddEntry(const FString& InEntryName, const FString& InEntryIcon, const FSimpleDelegate& InEntryEvent)
	{
		CHECK_STATE_MENU;
		FQEAddEntry Cmd;
		Cmd.EntryName = InEntryName;
		Cmd.EntryIcon = InEntryIcon;
		Cmd.EntryEvent = InEntryEvent;
		CmdBuffer.Write(Cmd);
	}

	void Menu::AddWidget(TSharedPtr<SWidget> InWidget)
	{
		CHECK_STATE_MENU;
		FQEAddWidget Cmd;
		Cmd.Widget = InWidget;
		CmdBuffer.Write(Cmd);
	}

	void Menu::BeginSection(const FString& InSecName)
	{
		CHECK_STATE_MENU;
		FQEBeginSection Cmd;
		Cmd.SectionName = InSecName;
		CmdBuffer.Write(Cmd);
	}

	void Menu::EndSection()
	{
		CHECK_STATE_MENU;
		FQEEndSection Cmd;
		CmdBuffer.Write(Cmd);
	}
}

#undef CHECK_STATE_MENU		
#undef CHECK_STATE_DETAIL		
#undef CHECK_STATE_ACTOR		
#undef CHECK_STATE_ASSET		
#undef CHECK_STATE_ASSET_NEW	
