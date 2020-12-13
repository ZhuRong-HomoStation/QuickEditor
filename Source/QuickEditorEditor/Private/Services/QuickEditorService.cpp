#include "QuickEditorService.h"

#include <typeindex>

#include "ContentBrowserModule.h"
#include "LevelEditor.h"
#include "Logging.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "Help/CommandResolvers.h"
#include "Help/MenuPathResolver.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

class FContentBrowserModule;

#define COND_ERR(Cond , FmtText, ...) while(!Cond) { UE_LOG(LogQuickEditor, Error, FmtText, __VA_ARGS__); break; }
#define CHECK_STATIC(UFun) COND_ERR(UFun->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static),\
		TEXT("%s: functions that contain Quick eidtor meta data must be static"), *UFun->GetName());

namespace QEPrivate
{
	const FName StyleSetName(TEXT("QEStyleSet"));
}


UQuickEditorService::UQuickEditorService()
{
}

void UQuickEditorService::Initialize(FSubsystemCollectionBase& Collection)
{
	// init style set 
	StyleSet = MakeShareable(new FSlateStyleSet(QEPrivate::StyleSetName));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	_LoadIcons();
	_InitPaths();
	_InitLevelEditMenu();
	_InitActorEditMenu();
	_InitAssetEditMenu();
	_InitToolBar();
}

void UQuickEditorService::Deinitialize()
{
	// shut down style set 
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
}

void UQuickEditorService::_InitPaths()
{
	// collect functions 
	for (TObjectIterator<UClass> ItClass; ItClass; ++ItClass)
	{
		UClass* TargetClass = *ItClass;
		if (ItClass->HasMetaData(TEXT("QEReroute")))
		{
			FString TargetClassName = ItClass->GetMetaData(TEXT("QEReroute"));
			TargetClass = FindObjectChecked<UClass>(ANY_PACKAGE, *TargetClassName);
		}
			
		for (TFieldIterator<UFunction> It(*ItClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
		{
			if (It->HasMetaData("QEMenuItem"))
			{
				CHECK_STATIC(It);
				
				FString Path = It->GetMetaData(TEXT("QEMenuItem"));

				LevelEditorPaths.AddPath(Path, *It, TargetClass);
			}
			else if (It->HasMetaData("QEAssetItem"))
			{
				CHECK_STATIC(It);

				FString Path = It->GetMetaData(TEXT("QEAssetItem"));

				AssetEditorPaths.AddPath(Path, *It, TargetClass);
			}
			else if (It->HasMetaData("QEActorItem"))
			{
				CHECK_STATIC(It);

				FString Path = It->GetMetaData(TEXT("QEActorItem"));

				ActorEditorPaths.AddPath(Path, *It, TargetClass);
			}
			else if (It->HasMetaData("QEToolBar"))
			{
				CHECK_STATIC(It);

				FString Path = It->GetMetaData(TEXT("QEToolBar"));

				ToolBarEditorPaths.AddPath(Path, *It, TargetClass);
			}
		}
	}

	LevelEditorPaths.SortPath();
	AssetEditorPaths.SortPath();
	ActorEditorPaths.SortPath();
}

void UQuickEditorService::_LoadIcons()
{
	// collect icons 
	for (TObjectIterator<UClass> ItClass; ItClass; ++ItClass)
	{
		for (TFieldIterator<UFunction> It(*ItClass); It; ++It)
		{
			if (It->HasMetaData(TEXT("QEIcon")))
			{
				CHECK_STATIC(It);
				FString IconPath = It->GetMetaData(TEXT("QEIcon"));

				// resolve path
				if (IconPath.Find(TEXT("::")) != INDEX_NONE) continue;		// style set case 
				FString Root;
				FString RelativePath;
				IconPath.Split(TEXT("/"), &Root, &RelativePath);
				if (Root == TEXT("Project"))
				{
					IconPath = FPaths::Combine(FPaths::ProjectDir(), RelativePath);
				}
				else
				{
					IconPath = FPaths::Combine(IPluginManager::Get().FindPlugin(Root)->GetBaseDir(), RelativePath);
				}
				IconPath = FPaths::ConvertRelativePathToFull(IconPath);

				// load icon
				FSlateImageBrush* Brush = new FSlateImageBrush(IconPath, FVector2D(40, 40));

				// combine style name
				FString StyleName = ItClass->GetName() + TEXT(".") + It->GetName();
				StyleSet->Set(FName(StyleName), Brush);
			}
		}
	}
}

void UQuickEditorService::_InitLevelEditMenu()
{	
	// add extensions
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(LevelEditorPaths.GetMenuBarExtender(TEXT("Help"), EExtensionHook::After));
}

void UQuickEditorService::_InitAssetEditMenu()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	auto& ExtendArr = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	
	ExtendArr.Add(FContentBrowserMenuExtender_SelectedAssets::CreateLambda([this](const TArray<FAssetData>& InAssets)
	{
		UClass* LowestClass = nullptr;
		for (auto& Asset : InAssets)
		{
			if (LowestClass == nullptr)
			{
				LowestClass = Asset.GetClass();
				continue;
			}
			if (LowestClass != Asset.GetClass() && LowestClass->IsChildOf(Asset.GetClass()))
			{
				LowestClass = Asset.GetClass();
			}
		}

		AssetEditorPaths.CurrentClass = LowestClass;
		AssetEditorPaths.RebuildNodes();
		return AssetEditorPaths.GetMenuExtender(TEXT("AssetContextSourceControl"), EExtensionHook::After).ToSharedRef();
	}));
}

void UQuickEditorService::_InitActorEditMenu()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	auto& MenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
	
	MenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateLambda(
		[this](const TSharedRef<FUICommandList> InCommandList, const TArray<AActor*> InActor)
	{
			UClass* LowestClass = nullptr;
            for (auto& Actor : InActor)
            {
                if (LowestClass == nullptr)
                {
                    LowestClass = Actor->GetClass();
                    continue;
                }
                if (LowestClass != Actor->GetClass() && LowestClass->IsChildOf(Actor->GetClass()))
                {
                    LowestClass = Actor->GetClass();
                }
            }

            ActorEditorPaths.CurrentClass = LowestClass;
			ActorEditorPaths.RebuildNodes();
            return ActorEditorPaths.GetMenuExtender(TEXT("ActorControl"), EExtensionHook::Before).ToSharedRef();
	}));
}

void UQuickEditorService::_InitToolBar()
{	
	// add extensions
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarEditorPaths.GetToolbarExtender(TEXT("Settings"), EExtensionHook::After));
}

void UQuickEditorService::_InitAssetNew()
{
	for (TObjectIterator<UClass> ItClass; ItClass; ++ItClass)
	{
		if (ItClass->HasMetaData(TEXT("QECreateNew")))
		{
			
		}
		else if (ItClass->HasMetaData(TEXT("QECreateFile")))
		{
			
		}
	}
}

#undef CHECK_STATIC
#undef COND_ERR
