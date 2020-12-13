#include "MenuPathResolver.h"
#include "CommandResolvers.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "Engine/Selection.h"

class FContentBrowserModule;
class FLevelEditorModule;

void FMenuNode::Sort()
{
	ChildNodes.Sort([](const FMenuNode& Lhs, const FMenuNode& Rhs)
	{
		if (Lhs.SectionName == Rhs.SectionName)
		{
			return Lhs.MenuName < Rhs.MenuName;
		}
		return Lhs.SectionName < Rhs.SectionName;
	});

	for (FMenuNode& Node : ChildNodes)
	{
		Node.Sort();
	}
}

void FMenuPathResolver::AddPath(const FString& InPath, UFunction* InFunction, UClass* InTargetClass, bool AddToFunctionInfos)
{
	// check params
	check(InFunction);
	check(InTargetClass);
	
	// validate path 
	check (FPaths::ValidatePath(InPath));

	// validate function type
	check (InFunction->NumParms == 0);
	check (InFunction->ReturnValueOffset == 65535);
	
	// find root
	FString NodeName;
	FString SectionName;
	FString EntryName;
	FString Path = InPath;
	if (!Path.Split(TEXT("/"), &NodeName, &Path))
	{
		NodeName = Path;
		Path.Reset();
	}
	if(!NodeName.Split(TEXT("."), &SectionName, &EntryName))
	{
		EntryName = NodeName;
	}
	FMenuNode* CurNode = EntryNodes.FindByPredicate(
		[&](const FMenuNode& InNode)
		{
			return InNode.MenuName == NodeName;
		});
	if (!CurNode)
	{
		EntryNodes.Emplace();
		CurNode = &EntryNodes.Top();
		CurNode->MenuName = NodeName;
		CurNode->SectionName = SectionName;
		CurNode->MenuName = EntryName;
	}

	// only root path
	if (Path.IsEmpty())
	{
		CurNode->BoundFunction = InFunction;
		// add to function map
		if (AddToFunctionInfos)
		{
			FString StyleSet, StyleName;
			if (InFunction->HasMetaData(TEXT("QEIcon")))
			{
				FString IconPath = InFunction->GetMetaData(TEXT("QEIcon"));
				if (!IconPath.Split(TEXT("::"), &StyleSet, &StyleName))
				{
					StyleSet = TEXT("QEStyleSet");
					StyleName = InFunction->GetOuter()->GetName() + TEXT(".") + InFunction->GetName();
				}
			}
			FMenuFunctionInfo Info;
			Info.Path = InPath;
			Info.StyleSet = MoveTemp(StyleSet);
			Info.StyleName = MoveTemp(StyleName);
			Info.TargetClass = InTargetClass;
			Info.bIsPopUp = InFunction->HasMetaData(TEXT("QEPopUp"));
			FunctionInfos.Add(InFunction, MoveTemp(Info));
		}
		return;
	}
	else
	{
		check(CurNode->BoundFunction == nullptr);
	}

	// build path node
	while (Path.Split(TEXT("/"), &NodeName, &Path))
	{
		if(!NodeName.Split(TEXT("."), &SectionName, &EntryName))
		{
			EntryName = NodeName;
		}

		// find exist node 
		FMenuNode* FoundNode = CurNode->ChildNodes.FindByPredicate(
			[&](const FMenuNode& InNode)
		{
			return InNode.MenuName == EntryName;
		});

		
		if (FoundNode)
		{
			// node conflict 
			check (FoundNode->BoundFunction == nullptr)
			CurNode = FoundNode;
		}
		else
		{
			// add child node
			CurNode->ChildNodes.Emplace(FMenuNode{ SectionName, EntryName });
			CurNode = &CurNode->ChildNodes.Top();
		}
	}

	// add menu entry
	check(!Path.IsEmpty());
	SectionName.Reset();
	EntryName.Reset();
	if (!Path.Split(TEXT("."), &SectionName, &EntryName))
	{
		EntryName = Path;
	}
	FMenuNode* FoundNode = CurNode->ChildNodes.FindByPredicate([&](const FMenuNode& InNode){ return InNode.MenuName == EntryName; });
	check(FoundNode == nullptr);	// node conflict
	FString StyleSet, StyleName;
	if (InFunction->HasMetaData(TEXT("QEIcon")))
	{
		FString IconPath = InFunction->GetMetaData(TEXT("QEIcon"));
		if (!IconPath.Split(TEXT("::"), &StyleSet, &StyleName))
		{
			StyleSet = TEXT("QEStyleSet");
			StyleName = InFunction->GetOuter()->GetName() + TEXT(".") + InFunction->GetName();
		}
	}

	CurNode->ChildNodes.Emplace(
		FMenuNode{
			MoveTemp(SectionName),
			MoveTemp(EntryName),
			{},
			InFunction});

	// add to function map
	if (AddToFunctionInfos)
	{
		FMenuFunctionInfo Info;
		Info.Path = InPath;
		Info.StyleSet = MoveTemp(StyleSet);
		Info.StyleName = MoveTemp(StyleName);
		Info.TargetClass = InTargetClass;
		Info.bIsPopUp = InFunction->HasMetaData(TEXT("QEPopUp"));
        FunctionInfos.Add(InFunction, MoveTemp(Info));
	}
}

void FMenuPathResolver::SortPath()
{
	EntryNodes.Sort([](const FMenuNode& Lhs, const FMenuNode& Rhs)
    {
        if (Lhs.SectionName == Rhs.SectionName)
        {
            return Lhs.MenuName < Rhs.MenuName;
        }
        return Lhs.SectionName < Rhs.SectionName;
    });
	for (FMenuNode& Node : EntryNodes)
	{
		Node.Sort();
	}
}

TSharedPtr<FExtender> FMenuPathResolver::GetToolbarExtender(FName InHookName, EExtensionHook::Position InPosition)
{
	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();

	Extender->AddToolBarExtension(
		InHookName,
		InPosition,
		nullptr,
		FToolBarExtensionDelegate::CreateRaw(this, &FMenuPathResolver::_ExtendToolBar));
	
	return Extender;
}

TSharedPtr<FExtender> FMenuPathResolver::GetMenuExtender(FName InHookName, EExtensionHook::Position InPosition)
{
	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();
	
	Extender->AddMenuExtension(
        InHookName,
        InPosition,
        nullptr,
        FMenuExtensionDelegate::CreateRaw(this, &FMenuPathResolver::_ExtendRootMenu,
        	const_cast<const TArray<FMenuNode>*>(&EntryNodes)));

	return Extender;
}

TSharedPtr<FExtender> FMenuPathResolver::GetMenuBarExtender(FName InHookName, EExtensionHook::Position InPosition)
{
	TSharedPtr<FExtender> Extender = MakeShared<FExtender>();

	for (FMenuNode& Node : EntryNodes)
	{
		Extender->AddMenuBarExtension(
			InHookName,
			InPosition,
			nullptr,
			FMenuBarExtensionDelegate::CreateRaw(this, &FMenuPathResolver::_ExtendMenuBar, &Node));
	}

	return Extender;
}

void FMenuPathResolver::_ExtendToolBar(FToolBarBuilder& InBuilder)
{
	for (FMenuNode& Node : EntryNodes)
	{
		if (Node.BoundFunction)
		{
			FMenuFunctionInfo& FuncInfo = FunctionInfos[Node.BoundFunction];
			if (FuncInfo.bIsPopUp)
			{
				InBuilder.AddComboButton(
                FUIAction(),
                FOnGetContent::CreateLambda([this, &Node]()
                {
					FMenuBuilder MenuBuilder(true,nullptr);
                	
					OnBeginPopUp(MenuBuilder);
					FFrame Stack(nullptr, Node.BoundFunction, nullptr);
					Node.BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEndPopUp(MenuBuilder);

                	return MenuBuilder.MakeWidget();
                }),
                FText::FromString(Node.MenuName),
                FText::GetEmpty());
			}
			else
			{
				InBuilder.AddToolBarButton(
                    FUIAction(FExecuteAction::CreateLambda([this, &Node]()
                    {
                        OnEntryCallBegin();
                        FFrame Stack(nullptr, Node.BoundFunction, nullptr);
                        Node.BoundFunction->Invoke(nullptr, Stack, nullptr);
                        OnEntryCallEnd();
                    })),
                    NAME_None,
                    FText::FromString(Node.MenuName),
                    FText::GetEmpty(),
                    FSlateIcon(*FuncInfo.StyleSet, *FuncInfo.StyleName));	
			}
		}
		else
		{
			InBuilder.AddComboButton(
				FUIAction(),
				FOnGetContent::CreateLambda([this, &Node]()
				{
					FMenuBuilder MenuBuilder(true, nullptr);
					_ExtendRootMenu(MenuBuilder, &Node.ChildNodes);
					return MenuBuilder.MakeWidget();
				}),
				FText::FromString(Node.MenuName),
				FText::GetEmpty());
		}
	}
}

void FMenuPathResolver::_ExtendMenu(FMenuBuilder& InBuilder, const FMenuNode* InNode)
{
	if (InNode->BoundFunction)
	{
		FMenuFunctionInfo& FuncInfo = FunctionInfos[InNode->BoundFunction];
		if (FuncInfo.bIsPopUp)
		{
			InBuilder.AddSubMenu(
				FText::FromString(InNode->MenuName),
				FText::GetEmpty(),
				FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
				{
					OnBeginPopUp(InBuilder);
					FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
					InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEndPopUp(InBuilder);
				}), false, FSlateIcon(*FuncInfo.StyleSet, *FuncInfo.StyleName));
		}
		else
		{
			InBuilder.AddMenuEntry(
				FText::FromString(InNode->MenuName),
				FText::GetEmpty(),
				FSlateIcon(*FuncInfo.StyleSet, *FuncInfo.StyleName),
	            FUIAction(FExecuteAction::CreateLambda([this, InNode]
	            {
					OnEntryCallBegin();
	                FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
	                InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEntryCallEnd();
	            })));
		}
	}
	else
	{
		InBuilder.AddSubMenu(FText::FromString(InNode->MenuName), FText::GetEmpty(),
        FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
        {
            if (!InNode->ChildNodes.Num()) return;

            // init section 
            FString Section = InNode->ChildNodes[0].SectionName;
            if (!Section.IsEmpty())
            {
                InBuilder.BeginSection(FName(Section), FText::FromString(Section));
            }

            // build menu 
            for (auto& ChildMenu : InNode->ChildNodes)
            {
                // change section 
                if (ChildMenu.SectionName != Section)
                {
                    if (!Section.IsEmpty())
                    {
                        InBuilder.EndSection();
                    }                			
                    Section = ChildMenu.SectionName;
                    if (!Section.IsEmpty())
                    {
                        InBuilder.BeginSection(FName(Section), FText::FromString(Section));
                    }
                }

                // build entry or submenu 
                _ExtendMenu(InBuilder, &ChildMenu);
            }
        }));
	}
}

void FMenuPathResolver::_ExtendRootMenu(FMenuBuilder& InBuilder, const TArray<FMenuNode>* InEntryNodes)
{
	if (!InEntryNodes->Num()) return;

	// init section 
	FString Section = (*InEntryNodes)[0].SectionName;
	if (!Section.IsEmpty())
	{
		InBuilder.BeginSection(FName(Section), FText::FromString(Section));
	}

	// build menu 
	for (auto& ChildMenu : *InEntryNodes)
	{
		// change section 
		if (ChildMenu.SectionName != Section)
		{
			if (!Section.IsEmpty())
			{
				InBuilder.EndSection();
			}                			
			Section = ChildMenu.SectionName;
			if (!Section.IsEmpty())
			{
				InBuilder.BeginSection(FName(Section), FText::FromString(Section));
			}
		}

		// build entry or submenu 
		_ExtendMenu(InBuilder, &ChildMenu);
	}
}

void FMenuPathResolver::_ExtendMenuBar(FMenuBarBuilder& InBuilder, FMenuNode* InNode)
{
	if (InNode->BoundFunction)
	{
		FMenuFunctionInfo& FuncInfo = FunctionInfos[InNode->BoundFunction];
		if (FuncInfo.bIsPopUp)
		{
			InBuilder.AddPullDownMenu(
				FText::FromString(InNode->MenuName),
				FText::GetEmpty(),
				FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
				{
					OnBeginPopUp(InBuilder);
					FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
					InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEndPopUp(InBuilder);
				}));
		}
		else
		{
			InBuilder.AddMenuEntry(
				FText::FromString(InNode->MenuName),
				FText::GetEmpty(),
				FSlateIcon(*FuncInfo.StyleSet, *FuncInfo.StyleName),
	            FUIAction(FExecuteAction::CreateLambda([this, InNode]
	            {
	            	OnEntryCallBegin();
	                FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
	                InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEntryCallEnd();
	            })));
		}
	}
	else
	{
		InBuilder.AddPullDownMenu(
			FText::FromString(InNode->MenuName),
			FText::GetEmpty(),
        FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
        {
            if (!InNode->ChildNodes.Num()) return;

            // init section 
            FString Section = InNode->ChildNodes[0].SectionName;
            if (!Section.IsEmpty())
            {
                InBuilder.BeginSection(FName(Section), FText::FromString(Section));
            }

            // build menu 
            for (auto& ChildMenu : InNode->ChildNodes)
            {
                // change section 
                if (ChildMenu.SectionName != Section)
                {
                    if (!Section.IsEmpty())
                    {
                        InBuilder.EndSection();
                    }                			
                    Section = ChildMenu.SectionName;
                    if (!Section.IsEmpty())
                    {
                        InBuilder.BeginSection(FName(Section), FText::FromString(Section));
                    }
                }

                // build entry or submenu 
                _ExtendMenu(InBuilder, &ChildMenu);
            }
        }));
	}
}

void FLevelMenuPathResolver::OnBeginPopUp(FMenuBuilder& InBuilder)
{
	QE::Menu::MenuState(true);
}

void FLevelMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	QE::CleanState();
	QEPrivate::ResolveMenuCommands(InBuilder);
	QE::CleanCommands();
}

void FActorMenuPathResolver::RebuildNodes()
{
	Reset(false);
	for (auto& Info : FunctionInfos)
	{
		if (CurrentClass->IsChildOf(Info.Value.TargetClass))
		{
			AddPath(Info.Value.Path, Info.Key, Info.Value.TargetClass, false);
		}
	}
}

void FActorMenuPathResolver::OnBeginPopUp(FMenuBuilder& InBuilder)
{
	QE::Menu::MenuState(true);
}

void FActorMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	QE::CleanState();
	QEPrivate::ResolveActorCommands(InBuilder);
	QE::CleanCommands();
}

void FActorMenuPathResolver::OnEntryCallBegin()
{
	GEditor->GetSelectedActors()->GetSelectedObjects(QE::SelectedActors);
}

void FActorMenuPathResolver::OnEntryCallEnd()
{
	QE::SelectedActors.Reset();
}

void FAssetMenuPathResolver::RebuildNodes()
{
	Reset(false);
	for (auto& Info : FunctionInfos)
	{
		if (CurrentClass->IsChildOf(Info.Value.TargetClass))
		{
			AddPath(Info.Value.Path, Info.Key, Info.Value.TargetClass, false);
		}
	}
}

void FAssetMenuPathResolver::OnBeginPopUp(FMenuBuilder& InBuilder)
{
	QE::Menu::MenuState(true);
}

void FAssetMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	QE::CleanState();
	QEPrivate::ResolveAssetCommands(InBuilder);
	QE::CleanCommands();
}

void FAssetMenuPathResolver::OnEntryCallBegin()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	for (FAssetData& Asset : SelectedAssets)
	{
		QE::SelectedAssets.Add(Asset.GetAsset());
	}
}

void FAssetMenuPathResolver::OnEntryCallEnd()
{
	QE::SelectedAssets.Reset();
}

void FToolBarPathResolver::OnBeginPopUp(FMenuBuilder& InBuilder)
{
	QE::Menu::MenuState(true);
}

void FToolBarPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	QE::CleanState();
	QEPrivate::ResolveMenuCommands(InBuilder);
	QE::CleanCommands();
}
