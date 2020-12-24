#pragma once
#include "CoreMinimal.h"

enum class EMenuWndType
{
	None ,
	Window ,
	DockTab ,
	Modal ,
};

struct FMenuNode
{
	FString				SectionName;
	FString				MenuName;
	TArray<FMenuNode>	ChildNodes;
	UFunction*			BoundFunction;
	FString				StyleSet;
	FString				StyleName;
	
	void Sort();
};

struct FMenuFunctionInfo
{
	FString			Path;
	bool			bIsPopUp = false;
	EMenuWndType	WndType = EMenuWndType::None;
	int32			WndNum;
	FString			ToolTip;
	UClass*			TargetClass;
};

class FMenuPathResolver
{
public:
	FMenuPathResolver() {}
	virtual ~FMenuPathResolver() {}
	
	void AddPath(const FString& InPath, UFunction* InFunction, UClass* InTargetClass, bool AddToFunctionInfos = true);
	void SetPathIcon(const FString& InPath, const FString& InIconPath);
	void SortPath();

	void Reset(bool ResetFunctionInfos = true)
	{
		EntryNodes.Reset();
		if (ResetFunctionInfos) FunctionInfos.Reset();
	}

	TSharedPtr<FExtender> GetToolbarExtender(FName InHookName, EExtensionHook::Position InPosition);
	TSharedPtr<FExtender> GetMenuExtender(FName InHookName, EExtensionHook::Position InPosition);
	TSharedPtr<FExtender> GetMenuBarExtender(FName InHookName, EExtensionHook::Position InPosition);
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) {}
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) {}
	virtual void OnEntryCallBegin() {}
	virtual void OnEntryCallEnd() {}

private:
	void _ExtendToolBar(FToolBarBuilder& InBuilder);
	void _ExtendMenu(FMenuBuilder& InBuilder, const FMenuNode* InNode);
	void _ExtendRootMenu(FMenuBuilder& InBuilder, const TArray<FMenuNode>* InEntryNodes);
	void _ExtendMenuBar(FMenuBarBuilder& InBuilder, FMenuNode* InNode);

	void _OpenWindow(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo);
	void _OpenDockTab(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo);
	void _OpenModalWindow(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo);

public:
	FString								Name;
	TMap<UFunction*, TArray<TSharedPtr<SWidget>>>	AllWnds;
	TArray<FMenuNode>					EntryNodes;
	TMap<UFunction*, FMenuFunctionInfo>	FunctionInfos;
};

class FLevelMenuPathResolver : public FMenuPathResolver
{
public:
	FLevelMenuPathResolver()
	{
		Name = TEXT("LevelMenu");
	}
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
};

class FActorMenuPathResolver : public FMenuPathResolver
{
public:
	FActorMenuPathResolver()
	{
		Name = TEXT("ActorMenu");
	}
	void RebuildNodes();
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEntryCallBegin() override;
	virtual void OnEntryCallEnd() override;
public:
	UClass*		CurrentClass;
};

class FAssetMenuPathResolver : public FMenuPathResolver
{
public:
	FAssetMenuPathResolver()
	{
		Name = TEXT("AssetMenu");
	}
	void RebuildNodes();
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEntryCallBegin() override;
	virtual void OnEntryCallEnd() override;
public:
	UClass*		CurrentClass;
};

class FToolBarPathResolver : public FMenuPathResolver
{
public:
	FToolBarPathResolver()
	{
		Name = TEXT("ToolBar");
	}
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
};

