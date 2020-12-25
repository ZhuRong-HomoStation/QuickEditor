#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"

#define CHECK_STATE_MENU		checkf(bIsMenuState, TEXT("function in QE::Menu can only called on MenuState"));
#define CHECK_STATE_DETAIL		checkf(bIsDetailState, TEXT("function in QE::Detail can only called on DetailState"));
#define CHECK_STATE_ACTOR		checkf(bIsActorState, TEXT("function in QE::Actor can only called on ActorState"));
#define CHECK_STATE_ASSET		checkf(bIsAssetState, TEXT("function in QE::Asset can only called on AssetState"));
#define CHECK_STATE_ASSET_NEW	checkf(bIsAssetNewState, TEXT("function in QE::AssetNew can only called on AssetNewState"));
#define CHECK_STATE_WINDOW		checkf(bIsWindowState, TEXT("function in QE::Window can only called on WindowState"));

namespace QE
{
	// Global 
	bool bIsMenuState = false;
	bool bIsDetailState = false;
	bool bIsActorState = false;
	bool bIsAssetState = false;
	bool bIsAssetNewState = false;
	bool bIsWindowState = false;
	FQECmdBuffer	CmdBuffer;

	// Actor action  
	TArray<AActor*>		SelectedActors;

	// Asset action 
	TArray<UObject*>	SelectedAssets;

	// Factory
	UClass*			NewObjectClass = nullptr;
	UObject*		NewObjectOuter = nullptr;
	FName			NewObjectName;
	EObjectFlags	NewObjectFlag;
	FString			ImportFileName;
	UObject*		NewCreatedObject = nullptr;
	bool			bIsReimportAsset = false;

	// Window
	int32					WindowId;
	FString					WindowName;
	TSharedPtr<SWidget>		WindowContent;
	FVector2D				WindowSize;
	
	// Detail
	TArray<UObject*>		SelectedObjects;
	
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

	void Window::WindowState(bool InEnable)
	{
		bIsWindowState = InEnable;
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

	FString AssetNew::GetImportFilePath()
	{
		CHECK_STATE_ASSET_NEW;
		return ImportFileName;
	}

	void AssetNew::CreatedObject(UObject* InObject)
	{
		CHECK_STATE_ASSET_NEW;
		NewCreatedObject = InObject;
	}

	UObject* AssetNew::CreatedObject()
	{
		CHECK_STATE_ASSET_NEW;
		return NewCreatedObject;
	}

	UObject* AssetNew::CreateDefault()
	{
		CHECK_STATE_ASSET_NEW;
		if (bIsReimportAsset) return NewCreatedObject;
		NewCreatedObject = NewObject<UObject>(
            NewObjectOuter,
            NewObjectClass,
            NewObjectName,
            NewObjectFlag);
		return NewCreatedObject;
	}

	bool AssetNew::IsReimport()
	{
		CHECK_STATE_ASSET_NEW;
		return bIsReimportAsset;
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

	int32 Window::GetID()
	{
		CHECK_STATE_WINDOW;
		return WindowId;
	}

	void Window::WndName(const FString& InName)
	{
		CHECK_STATE_WINDOW;
		WindowName = InName;
	}

	void Window::WndContent(TSharedRef<SWidget> InWidget)
	{
		CHECK_STATE_WINDOW;
		WindowContent = InWidget;
	}

	void Window::WndSize(FVector2D Size)
	{
		CHECK_STATE_WINDOW;
		WindowSize = Size;
	}

	const TArray<UObject*>& Detail::GetSelectedObjects()
	{
		CHECK_STATE_DETAIL;
		return SelectedObjects;
	}

	void Detail::HideParent(const FString& InName)
	{
		CHECK_STATE_DETAIL;
		FQEHideParent Cmd;
		Cmd.ParentName = InName;
		CmdBuffer.Write(Cmd);
	}

	void Detail::ShowParent(const FString& InName)
	{
		CHECK_STATE_DETAIL;
		FQEShowParent Cmd;
		Cmd.ParentName = InName;
		CmdBuffer.Write(Cmd);
	}

	void Detail::HideCategory(const FString& InCategory)
	{
		CHECK_STATE_DETAIL;
		FQEHideCategory Cmd;
		Cmd.Category = InCategory;
		CmdBuffer.Write(Cmd);
	}

	void Detail::EditCategory(const FString& InCategory, int32 InPriority)
	{
		CHECK_STATE_DETAIL;
		FQEEditCategory Cmd;
		Cmd.Category = InCategory;
		Cmd.Priority = InPriority;
		CmdBuffer.Write(Cmd);
	}

	void Detail::HideProperty(const FString& InPropertyName)
	{
		CHECK_STATE_DETAIL;
		FQEHideProperty Cmd;
		Cmd.PropertyName = InPropertyName;
		CmdBuffer.Write(Cmd);
	}

	void Detail::EditProperty(const FString& InPropertyName, TSharedPtr<SWidget> InWidget)
	{
		CHECK_STATE_DETAIL;
		FQEEditProperty Cmd;
		Cmd.PropertyName = InPropertyName;
		Cmd.ValueWidget = InWidget;
		CmdBuffer.Write(Cmd);
	}

	void Detail::EditProperty(const FString& InPropertyName, TSharedPtr<SWidget> InWidget,
		const FString& InOverrideName)
	{
		CHECK_STATE_DETAIL;
		FQEEditProperty Cmd;
		Cmd.PropertyName = InPropertyName;
		Cmd.OverrideName = InOverrideName;
		Cmd.ValueWidget = InWidget;
		CmdBuffer.Write(Cmd);
	}

	void Detail::EditProperty(const FString& InPropertyName, TSharedPtr<SWidget> InValueWidget,
		TSharedPtr<SWidget> InNameWidget)
	{
		CHECK_STATE_DETAIL;
		FQEEditProperty Cmd;
		Cmd.PropertyName = InPropertyName;
		Cmd.NameWidget = InNameWidget;
		Cmd.ValueWidget = InValueWidget;
		CmdBuffer.Write(Cmd);
	}

	void Detail::AddWidget(const FString& InSearchName, TSharedPtr<SWidget> InWidget)
	{
		CHECK_STATE_DETAIL;
		FQEDetailAddWidget Cmd;
		Cmd.SearchName = InSearchName;
		Cmd.ValueWidget = InWidget;
		CmdBuffer.Write(Cmd);
	}

	void Detail::AddWidget(const FString& InSearchName, TSharedPtr<SWidget> InWidget, const FString& InName)
	{
		CHECK_STATE_DETAIL;
		FQEDetailAddWidget Cmd;
		Cmd.WidgetName = InName;
		Cmd.SearchName = InSearchName;
		Cmd.ValueWidget = InWidget;
		CmdBuffer.Write(Cmd);
	}

	void Detail::AddWidget(const FString& InSearchName, TSharedPtr<SWidget> InValueWidget,
		TSharedPtr<SWidget> InNameWidget)
	{
		CHECK_STATE_DETAIL;
		FQEDetailAddWidget Cmd;
		Cmd.SearchName = InSearchName;
		Cmd.NameWidget = InNameWidget;
		Cmd.ValueWidget = InValueWidget;
		CmdBuffer.Write(Cmd);
	}
}

#undef CHECK_STATE_MENU		
#undef CHECK_STATE_DETAIL		
#undef CHECK_STATE_ACTOR		
#undef CHECK_STATE_ASSET		
#undef CHECK_STATE_ASSET_NEW
#undef CHECK_STATE_WINDOW
