#include "MenuPathResolver.h"
#include "CommandResolvers.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "Engine/Selection.h"
#include "Services/QuickEditorService.h"

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

			FString WndNum, WndType;
			if (InFunction->HasMetaData(TEXT("QEWindow")))
			{
				FString WndConfig = InFunction->GetMetaData(TEXT("QEWindow"));
				if (!WndConfig.Split(TEXT("|"), &WndNum, &WndType))
				{
					WndNum = TEXT("1");
					WndType = MoveTemp(WndConfig);
				}
			}
			
			FMenuFunctionInfo Info;
			Info.Path = InPath;
			CurNode->StyleSet = MoveTemp(StyleSet);
			CurNode->StyleName = MoveTemp(StyleName);
			Info.TargetClass = InTargetClass;
			Info.ToolTip = InFunction->GetMetaData("ToolTip");
			Info.bIsPopUp = InFunction->HasMetaData(TEXT("QEPopUp"));
			Info.WndNum = FCString::Atoi(*WndNum);
			if (WndType == TEXT("Win"))
			{
				Info.WndType = EMenuWndType::Window;
			}
			else if (WndType == TEXT("Dock"))
			{
				Info.WndType = EMenuWndType::DockTab;
			}
			else if (WndType == TEXT("Modal"))
			{
				Info.WndType = EMenuWndType::Modal;
			}
			else if (Info.WndNum != 0)
			{
				checkNoEntry();
			}
			if (Info.WndNum > 0)
			{
				AllWnds.Add(InFunction).SetNum(Info.WndNum);
			}
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
	CurNode = &CurNode->ChildNodes.Top();
	
	// add to function map
	if (AddToFunctionInfos)
	{
		FString WndNum, WndType;
		if (InFunction->HasMetaData(TEXT("QEWindow")))
		{
			FString WndConfig = InFunction->GetMetaData(TEXT("QEWindow"));
			if (!WndConfig.Split(TEXT("|"), &WndNum, &WndType))
			{
				WndNum = TEXT("1");
				WndType = MoveTemp(WndConfig);
			}
		}
		
		FMenuFunctionInfo Info;
		Info.Path = InPath;
		CurNode->StyleSet = MoveTemp(StyleSet);
		CurNode->StyleName = MoveTemp(StyleName);
		Info.TargetClass = InTargetClass;
		Info.ToolTip = InFunction->GetMetaData("ToolTip");
		Info.bIsPopUp = InFunction->HasMetaData(TEXT("QEPopUp"));
		Info.WndNum = FCString::Atoi(*WndNum);
		if (WndType == TEXT("Win"))
		{
			Info.WndType = EMenuWndType::Window;
		}
		else if (WndType == TEXT("Dock"))
		{
			Info.WndType = EMenuWndType::DockTab;
		}
		else if (WndType == TEXT("Modal"))
		{
			Info.WndType = EMenuWndType::Modal;
		}
		else if (Info.WndNum != 0)
		{
			checkNoEntry();
		}
		if (Info.WndNum > 0)
		{
			AllWnds.Add(InFunction).SetNum(Info.WndNum);
		}
        FunctionInfos.Add(InFunction, MoveTemp(Info));
	}
}

void FMenuPathResolver::SetPathIcon(const FString& InPath, const FString& InIconPath)
{
	FString Path = InPath;
	FString NodeName;
	FMenuNode* CurNode = nullptr;
	TArray<FMenuNode>* NodeArray = &EntryNodes;
	
	auto FindNode = [&NodeName](const FMenuNode& InNode)
	{
		return InNode.MenuName == NodeName;
	};
	
	// init root node 
	if (!Path.Split(TEXT("/"), &NodeName, &Path))
	{
		NodeName = MoveTemp(Path);
	}

	// search end node
	do
	{
		NodeName.Split(TEXT("."), nullptr, &NodeName);
		CurNode = NodeArray->FindByPredicate(FindNode);
		if (!CurNode) return;
		NodeArray = &CurNode->ChildNodes;
	} while (Path.Split(TEXT("/"), &NodeName, &Path));

	// get end node
	if (!Path.IsEmpty())
	{
		NodeName = MoveTemp(Path);
		NodeName.Split(TEXT("."), nullptr, &NodeName);
		CurNode = NodeArray->FindByPredicate(FindNode);
		if (!CurNode) return;
	}
	if (!InIconPath.Split(TEXT("::"), &CurNode->StyleSet, &CurNode->StyleName))
	{
		CurNode->StyleSet = TEXT("QEStyleSet");
		CurNode->StyleName = TEXT("Path.")+ InPath;
		UQuickEditorService::Get().AddIcon(CurNode->StyleName, InIconPath);
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
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*Node.StyleSet, *Node.StyleName));
			}
			else if (FuncInfo.WndNum > 0)
			{
				switch (FuncInfo.WndType)
				{
				case EMenuWndType::Window:
					InBuilder.AddToolBarButton(
                    FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenWindow, const_cast<const FMenuNode*>(&Node), &FuncInfo)),
                    NAME_None,
                    FText::FromString(Node.MenuName),
                    FText::FromString(FuncInfo.ToolTip),
                    FSlateIcon(*Node.StyleSet, *Node.StyleName));	
					break;
				case EMenuWndType::DockTab:
					InBuilder.AddToolBarButton(
                    FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenDockTab, const_cast<const FMenuNode*>(&Node), &FuncInfo)),
                    NAME_None,
                    FText::FromString(Node.MenuName),
                    FText::FromString(FuncInfo.ToolTip),
                    FSlateIcon(*Node.StyleSet, *Node.StyleName));	
					break;
				case EMenuWndType::Modal:
					InBuilder.AddToolBarButton(
                    FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenModalWindow, const_cast<const FMenuNode*>(&Node), &FuncInfo)),
                    NAME_None,
                    FText::FromString(Node.MenuName),
                    FText::FromString(FuncInfo.ToolTip),
                    FSlateIcon(*Node.StyleSet, *Node.StyleName));	
					break;
				default: checkNoEntry();
				}
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
                    FText::FromString(FuncInfo.ToolTip),
                    FSlateIcon(*Node.StyleSet, *Node.StyleName));	
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
				FText::GetEmpty(),
				FSlateIcon(*Node.StyleSet, *Node.StyleName));
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
				FText::FromString(FuncInfo.ToolTip),
				FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
				{
					OnBeginPopUp(InBuilder);
					FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
					InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEndPopUp(InBuilder);
				}), false, FSlateIcon(*InNode->StyleSet, *InNode->StyleName));
		}
		else if (FuncInfo.WndNum > 0)
		{
			switch (FuncInfo.WndType)
			{
			case EMenuWndType::Window:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenWindow, InNode, &FuncInfo)));
				break;
			case EMenuWndType::DockTab:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenDockTab, InNode, &FuncInfo)));
				break;
			case EMenuWndType::Modal:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenModalWindow, InNode, &FuncInfo)));
				break;
			default: checkNoEntry();
			}
		}
		else
		{
			InBuilder.AddMenuEntry(
				FText::FromString(InNode->MenuName),
				FText::FromString(FuncInfo.ToolTip),
				FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
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
        }),false, FSlateIcon(*InNode->StyleSet, *InNode->StyleName));
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
				FText::FromString(FuncInfo.ToolTip),
				FNewMenuDelegate::CreateLambda([this, InNode](FMenuBuilder& InBuilder)
				{
					OnBeginPopUp(InBuilder);
					FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
					InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
					OnEndPopUp(InBuilder);
				}));
		}
		else if (FuncInfo.WndNum > 0)
		{
			switch (FuncInfo.WndType)
			{
			case EMenuWndType::Window:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenWindow, const_cast<const FMenuNode*>(InNode), &FuncInfo)));
				break;
			case EMenuWndType::DockTab:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenDockTab, const_cast<const FMenuNode*>(InNode), &FuncInfo)));
				break;
			case EMenuWndType::Modal:
				InBuilder.AddMenuEntry(
                FText::FromString(InNode->MenuName),
                FText::FromString(FuncInfo.ToolTip),
                FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
                FUIAction(FExecuteAction::CreateRaw(this, &FMenuPathResolver::_OpenModalWindow, const_cast<const FMenuNode*>(InNode), &FuncInfo)));
				break;
			default: checkNoEntry();
			}
		}
		else
		{
			InBuilder.AddMenuEntry(
				FText::FromString(InNode->MenuName),
				FText::FromString(FuncInfo.ToolTip),
				FSlateIcon(*InNode->StyleSet, *InNode->StyleName),
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

void FMenuPathResolver::_OpenWindow(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo)
{
	// search wnd 
	auto& WndArr = AllWnds[InNode->BoundFunction];
	QE::WindowId = -1;
	for (int32 i = 0; i < InFuncInfo->WndNum; ++i)
	{
		if (!WndArr[i].IsValid())
		{
			QE::WindowId = i;
			break;
		}
	}

	// no wnd
	if (QE::WindowId == -1)
	{
		StaticCastSharedPtr<SWindow>(WndArr[0])->BringToFront();
		return;
	}

	// invoke 
	QE::Window::WindowState(true);
	FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
	InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
	QE::Window::WindowState(false);
	FString WndName = MoveTemp(QE::WindowName);
	TSharedPtr<SWidget> WndContent = MoveTemp(QE::WindowContent);

	// create wnd
	auto NewWnd = SNew(SWindow)
    .Title(FText::FromString(WndName))
    .SizingRule(ESizingRule::UserSized)
    .ClientSize(QE::WindowSize)
    [
        WndContent.ToSharedRef()
    ];
	NewWnd->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&WndArr, Idx=QE::WindowId](const TSharedRef<SWindow>&)
    {
        WndArr[Idx].Reset();
    }));

	// storage and add show
	WndArr[QE::WindowId] = NewWnd;
	FSlateApplication::Get().AddWindow(NewWnd); 
}

void FMenuPathResolver::_OpenDockTab(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo)
{
	// search wnd 
    auto& WndArr = AllWnds[InNode->BoundFunction];
    QE::WindowId = -1;
    for (int32 i = 0; i < InFuncInfo->WndNum; ++i)
    {
        if (!WndArr[i].IsValid())
        {
            QE::WindowId = i;
            break;
        }
    }

    // no wnd
    if (QE::WindowId == -1)
    {
        auto Wnd = StaticCastSharedPtr<SDockTab>(WndArr[0])->GetParentWindow();
    	if (Wnd) Wnd->BringToFront();
        StaticCastSharedPtr<SDockTab>(WndArr[0])->ActivateInParent(ETabActivationCause::SetDirectly);
        StaticCastSharedPtr<SDockTab>(WndArr[0])->FlashTab();
        return;
    }
    
    // invoke 
    QE::Window::WindowState(true);
    FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
    InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
    QE::Window::WindowState(false);
    FString WndName = MoveTemp(QE::WindowName);
    TSharedPtr<SWidget> WndContent = MoveTemp(QE::WindowContent);

    // create tab 
	auto NewTab = SNew(SDockTab)
    .Label(FText::FromString(WndName))
    .TabRole(ETabRole::NomadTab)
    [
        WndContent.ToSharedRef()
    ];
    NewTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([&WndArr, Idx=QE::WindowId](TSharedRef<SDockTab>)
    {
        WndArr[Idx].Reset();
    }));

    // storage and show 
    FName PlaceholderId(TEXT("StandaloneToolkit"));
    WndArr[QE::WindowId] = NewTab;
    TSharedPtr<FTabManager::FSearchPreference> SearchPreference = MakeShareable(new FTabManager::FRequireClosedTab());
    FGlobalTabmanager::Get()->InsertNewDocumentTab(PlaceholderId, *SearchPreference, NewTab);
}

void FMenuPathResolver::_OpenModalWindow(const FMenuNode* InNode, FMenuFunctionInfo* InFuncInfo)
{
	// invoke 
	QE::Window::WindowState(true);
	QE::WindowId = 0;
	FFrame Stack(nullptr, InNode->BoundFunction, nullptr);
	InNode->BoundFunction->Invoke(nullptr, Stack, nullptr);
	QE::Window::WindowState(false);
	FString WndName = MoveTemp(QE::WindowName);
	TSharedPtr<SWidget> WndContent = MoveTemp(QE::WindowContent);

	// create modal wnd 
	TSharedPtr<SWindow> NewWnd = SNew(SWindow)
    .Title(FText::FromString(WndName))
    .SizingRule(ESizingRule::UserSized)
    .ClientSize(QE::WindowSize)
    [
        WndContent.ToSharedRef()
    ];
	FSlateApplication::Get().AddModalWindow(NewWnd.ToSharedRef(), nullptr);
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
	OnEntryCallBegin();
}

void FActorMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	OnEntryCallEnd();
	QE::CleanState();
	QEPrivate::ResolveActorCommands(InBuilder);
	QE::CleanCommands();
}

void FActorMenuPathResolver::OnEntryCallBegin()
{
	GEditor->GetSelectedActors()->GetSelectedObjects(QE::SelectedActors);
	QE::Actor::ActorState(true);
}

void FActorMenuPathResolver::OnEntryCallEnd()
{
	QE::Actor::ActorState(false);
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
	OnEntryCallBegin();
}

void FAssetMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	OnEntryCallEnd();
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
	QE::Asset::AssetState(true);
}

void FAssetMenuPathResolver::OnEntryCallEnd()
{
	QE::SelectedAssets.Reset();
	QE::Asset::AssetState(false);
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

void FAssetEditorMenuPathResolver::RebuildNodes()
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

void FAssetEditorMenuPathResolver::OnBeginPopUp(FMenuBuilder& InBuilder)
{
	QE::Menu::MenuState(true);
	OnEntryCallBegin();
}

void FAssetEditorMenuPathResolver::OnEndPopUp(FMenuBuilder& InBuilder)
{
	OnEntryCallEnd();
	QE::CleanState();
	QEPrivate::ResolveAssetCommands(InBuilder);
	QE::CleanCommands();
}

void FAssetEditorMenuPathResolver::OnEntryCallBegin()
{
	// TODO: collect selected assets 
	QE::Asset::AssetState(true);
}

void FAssetEditorMenuPathResolver::OnEntryCallEnd()
{
	QE::SelectedAssets.Reset();
	QE::Asset::AssetState(false);
}
