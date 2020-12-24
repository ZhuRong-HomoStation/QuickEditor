#pragma once
#include "CoreMinimal.h"

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
	FString		Path;
	bool		bIsPopUp = false;
	FString		ToolTip;
	UClass*		TargetClass;
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
	
public:
	TArray<FMenuNode>					EntryNodes;
	TMap<UFunction*, FMenuFunctionInfo>	FunctionInfos;
};

class FLevelMenuPathResolver : public FMenuPathResolver
{
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
};

class FActorMenuPathResolver : public FMenuPathResolver
{
public:
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
protected:
	virtual void OnBeginPopUp(FMenuBuilder& InBuilder) override;
	virtual void OnEndPopUp(FMenuBuilder& InBuilder) override;
};

