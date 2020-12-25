#include "QuickEditorService.h"
#include "AssetToolsModule.h"
#include "AssetTypeCategories.h"
#include "ContentBrowserModule.h"
#include "EditorReimportHandler.h"
#include "LevelEditor.h"
#include "Logging.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "Config/QuickEditorConfig.h"
#include "Help/CommandResolvers.h"
#include "Help/MenuPathResolver.h"
#include "Help/QEAssetTypeAction.h"
#include "Help/QEFactories.h"
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
	// IHotReloadInterface& HotReloadSupport = FModuleManager::LoadModuleChecked<IHotReloadInterface>("HotReload");
	// HotReloadSupport.OnHotReload().AddRaw(this, &FGraphNodeClassHelper::OnHotReload);
}

void UQuickEditorService::AddIcon(const FString& InName, const FString& InPath, FVector2D Size)
{
	if (StyleSet->GetBrush(FName(InName)) != StyleSet->GetDefaultBrush())
		return;
	FString Path = InPath;
	if (FPaths::IsRelative(InPath))
	{
		Path = ResolveIconPath(InPath);
	}
	FSlateImageBrush* Brush = new FSlateImageBrush(Path, Size);

	// combine style name 
	StyleSet->Set(FName(InName), Brush);
}

FString UQuickEditorService::ResolveIconPath(const FString& InPath)
{
	FString IconPath = InPath;
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
	return FPaths::ConvertRelativePathToFull(IconPath);
}

void UQuickEditorService::Initialize(FSubsystemCollectionBase& Collection)
{
	// init style set 
	StyleSet = MakeShareable(new FSlateStyleSet(QEPrivate::StyleSetName));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	
	_LoadIcons();
	_InitPaths();
	_InitPathIcons();
	
	_InitLevelEditMenu();
	_InitActorEditMenu();
	_InitAssetEditMenu();

	_InitToolBar();
	_InitAssetNew();
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
				FString IconSize = It->GetMetaData(TEXT("QEIconSize"));

				if (It->HasMetaData(TEXT("QECreateNew")) || It->HasMetaData(TEXT("QECreateFile")))
				{
					_ResolveThumbnail(IconPath, IconSize, *It);
				}
				else
				{
					_ResolveIcon(IconPath, IconSize, *It);
				}
			}
		}
	}
}

void UQuickEditorService::_InitPathIcons()
{
	UQuickEditorConfig* Config = UQuickEditorConfig::Get();

	for (auto& Icon : Config->MainMenuPathIcons)
	{
		LevelEditorPaths.SetPathIcon(Icon.Key, Icon.Value);
	}

	for (auto& Icon : Config->ActorActionPathIcons)
	{
		ActorEditorPaths.SetPathIcon(Icon.Key, Icon.Value);
	}

	for (auto& Icon : Config->AssetActionPathIcons)
	{
		AssetEditorPaths.SetPathIcon(Icon.Key, Icon.Value);
	}

	for (auto& Icon : Config->ToolBarPathIcons)
	{
		ToolBarEditorPaths.SetPathIcon(Icon.Key, Icon.Value);
	}
}

void UQuickEditorService::_ResolveIcon(FString IconPath, FString IconSize, UFunction* InFunc)
{
	// resolve path
	if (IconPath.Find(TEXT("::")) != INDEX_NONE) return;		// style set case 

	// resolve path  
	IconPath = ResolveIconPath(IconPath);

	// load icon 
	FVector2D Size(40, 40);
	if (!IconSize.IsEmpty())
	{
		FString XSize, YSize;
		IconSize.Split(TEXT(","), &XSize, &YSize);
		Size.X = FCString::Atof(*XSize);
		Size.Y = FCString::Atof(*YSize);
	}
	FSlateImageBrush* Brush = new FSlateImageBrush(IconPath, Size);

	// combine style name
	FString StyleName = InFunc->GetOuter()->GetName() + TEXT(".") + InFunc->GetName();
	StyleSet->Set(FName(StyleName), Brush);
}

void UQuickEditorService::_ResolveThumbnail(FString IconPath, FString IconSize, UFunction* InFunc)
{
	// get class name
	UClass* Class = (UClass*)InFunc->GetOuter();
	FString ClassName = Class->GetName();
	if (Class->HasMetaData(TEXT("QEReroute")))
	{
		ClassName = Class->GetMetaData(TEXT("QEReroute"));
	}
	
	// resolve path
	if (IconPath.Find(TEXT("::")) != INDEX_NONE)
	{
		FSlateBrush* Brush = nullptr;
		{
			FString TempStyleSet, TempStyleName;
			IconPath.Split(TEXT("::"), &TempStyleSet, &TempStyleName);
			FSlateIcon Icon(*TempStyleSet, *TempStyleName);
			Brush = new FSlateBrush(*Icon.GetOptionalIcon());
		}

		// combine style name
		FString StyleName = TEXT("ClassThumbnail.") + ClassName;
		StyleSet->Set(FName(StyleName), Brush);
	}
	else
	{
		// resolve path  
		IconPath = ResolveIconPath(IconPath);

		// load icon 
		FVector2D Size(40, 40);
		if (!IconSize.IsEmpty())
		{
			FString XSize, YSize;
			IconSize.Split(TEXT(","), &XSize, &YSize);
			Size.X = FCString::Atof(*XSize);
			Size.Y = FCString::Atof(*YSize);
		}
		FSlateImageBrush* Brush = new FSlateImageBrush(IconPath, Size);

		// combine style name
		FString StyleName = TEXT("ClassThumbnail.") + ClassName;
		StyleSet->Set(FName(StyleName), Brush);
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
	// load asset tools 
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// init map
	TMap<FName, EAssetTypeCategories::Type> Categories;
	Categories.Reserve(32);
	Categories.Add(TEXT("Basic"), EAssetTypeCategories::Basic);
	Categories.Add(TEXT("Animation"), EAssetTypeCategories::Animation);
	Categories.Add(TEXT("MaterialsAndTextures"), EAssetTypeCategories::MaterialsAndTextures);
	Categories.Add(TEXT("Sounds"), EAssetTypeCategories::Sounds);
	Categories.Add(TEXT("Physics"), EAssetTypeCategories::Physics);
	Categories.Add(TEXT("UI"), EAssetTypeCategories::UI);
	Categories.Add(TEXT("Misc"), EAssetTypeCategories::Misc);
	Categories.Add(TEXT("Gameplay"), EAssetTypeCategories::Gameplay);
	Categories.Add(TEXT("Blueprint"), EAssetTypeCategories::Blueprint);
	Categories.Add(TEXT("Media"), EAssetTypeCategories::Media);
	
	// collect categories and classes
	struct FFactoryInfo
	{
		UFunction*					NewFactory = nullptr;
		EAssetTypeCategories::Type	Category;
		FString						NewItemName;
		
		UFunction*					FileFactory = nullptr;
		TArray<FString>				SupportFileExtensions;
	};
	TMap<UClass*, FFactoryInfo> ClassFactoryMap;
	for (TObjectIterator<UClass> ItClass; ItClass; ++ItClass)
	{
		UClass* TargetClass = *ItClass;
		FFactoryInfo FactoryInfo;

		// reroute 
		if (ItClass->HasMetaData(TEXT("QEReroute")))
		{
			TargetClass = FindObjectChecked<UClass>(ANY_PACKAGE, *ItClass->GetMetaData(TEXT("QEReroute")));
		}

		// search factories 
		for (TFieldIterator<UFunction> ItFunc(*ItClass); ItFunc; ++ItFunc)
		{
			if (ItFunc->HasMetaData(TEXT("QECreateNew")))
			{
				CHECK_STATIC(ItFunc);
				checkf(FactoryInfo.NewFactory == nullptr, TEXT("class %s: QECreateNew function conflict"), *ItClass->GetName());
				FactoryInfo.NewFactory = *ItFunc;
			}
			else if (ItFunc->HasMetaData(TEXT("QECreateFile")))
			{
				CHECK_STATIC(ItFunc);
				checkf(FactoryInfo.FileFactory == nullptr, TEXT("class %s: QECreateFile function conflict"), *ItClass->GetName());
				FactoryInfo.FileFactory = *ItFunc;
			}
		}

		// parse factories
		FFactoryInfo* Info = ClassFactoryMap.Find(TargetClass);
		if ((FactoryInfo.NewFactory || FactoryInfo.FileFactory) && !Info)
		{
			Info = &ClassFactoryMap.Add(TargetClass);
		}
		if (Info)
		{
			Info->NewItemName = TargetClass->GetName();
		}
		
		if (FactoryInfo.NewFactory)
		{
			FString Value = FactoryInfo.NewFactory->GetMetaData(TEXT("QECreateNew"));
			FString Category, Name;
			if (!Value.Split(TEXT("::"), &Category, &Name))
			{
				Category = TEXT("Misc");
				Name = Value;
			}

			// register category
			EAssetTypeCategories::Type* FoundCate = Categories.Find(FName(Category));
			if (!FoundCate)
			{
				EAssetTypeCategories::Type NewCate = AssetTools.RegisterAdvancedAssetCategory(
					FName(Category),
					FText::FromString(Category));
				FoundCate = &Categories.Add(FName(Category), NewCate);
			}
			FactoryInfo.Category = *FoundCate;
			FactoryInfo.NewItemName = Name;
			
			// add to map 
			checkf(Info->NewFactory == nullptr, TEXT("Class %s QECreateNew Confilict!!!"), *TargetClass->GetName());
			Info->Category = FactoryInfo.Category;
			Info->NewFactory = FactoryInfo.NewFactory;
			Info->NewItemName = FactoryInfo.NewItemName;
		}
		if (FactoryInfo.FileFactory)
		{
			FString Value = FactoryInfo.FileFactory->GetMetaData(TEXT("QECreateFile"));
			FString Token;

			// collect files 
			while (Value.Split(TEXT(","), &Token, &Value))
			{
				FactoryInfo.SupportFileExtensions.Add(Token);
			}
			FactoryInfo.SupportFileExtensions.Add(Value);
			
			// add to map
			checkf(Info->FileFactory == nullptr, TEXT("Class %s QECreateFile Confilict!!!"), *TargetClass->GetName());
			Info->FileFactory = FactoryInfo.FileFactory;
			Info->SupportFileExtensions = FactoryInfo.SupportFileExtensions;
		}
	}

	// build factories
	for (auto& Item : ClassFactoryMap)
	{
		// add AssetTypeAction 
		TSharedPtr<FQEAssetTypeAction> AssetAction = MakeShared<FQEAssetTypeAction>();
		AssetAction->Name = Item.Value.NewItemName;
		AssetAction->SupportClass = Item.Key;
		AssetAction->AssetCategory = Item.Value.Category;
		AssetAction->bIsImportedAsset = Item.Value.FileFactory != nullptr;
		AssetTools.RegisterAssetTypeActions(AssetAction.ToSharedRef());
		
		if (Item.Value.NewFactory)
		{
			// add factory
			UClass* NewFactoryClass = NewObject<UClass>(GetTransientPackage(), FName(Item.Key->GetName() + TEXT("FactoryNew")),
				RF_Standalone);
			NewFactoryClass->SetSuperStruct(UQEFactoryNew::StaticClass());
			NewFactoryClass->Bind();
			NewFactoryClass->StaticLink(true);
			NewFactoryClass->AssembleReferenceTokenStream();
			NewFactoryClass->AddToRoot();
			
			// create default object 
			UQEFactoryNew* NewFactory = Cast<UQEFactoryNew>(NewFactoryClass->GetDefaultObject());
			NewFactory->AddToRoot();
			NewFactory->SupportedClass = Item.Key;
			UQEFactoryNew::NewFunctionMap.Add(NewFactoryClass, Item.Value.NewFactory);
			UQEFactoryNew::SupportClassMap.Add(NewFactoryClass, Item.Key);
		}
		
		if (Item.Value.FileFactory)
		{
			// add factory
			UClass* FileFactoryClass = NewObject<UClass>(GetTransientPackage(), FName(Item.Key->GetName() + TEXT("FactoryFile")),
                RF_Standalone);
			FileFactoryClass->SetSuperStruct(UQEFactoryFile::StaticClass());
			FileFactoryClass->Bind();
			FileFactoryClass->StaticLink(true);
			FileFactoryClass->AssembleReferenceTokenStream();
			FileFactoryClass->AddToRoot();
			
			// create default object 
			UQEFactoryFile* FileFactory = Cast<UQEFactoryFile>(FileFactoryClass->GetDefaultObject());
			FileFactory->AddToRoot();
			FileFactory->SupportedClass = Item.Key;
			for (auto& Extension : Item.Value.SupportFileExtensions)
			{
				FileFactory->Formats.Add(Extension + TEXT("; Import file"));
			}
			UQEFactoryFile::NewFunctionMap.Add(FileFactoryClass, Item.Value.FileFactory);
			UQEFactoryFile::SupportClassMap.Add(FileFactoryClass, Item.Key);
		}
	}
}

void UQuickEditorService::_InitDetail()
{
	// DONE: inject hack property
	// DONE: register detail extension
	// DONE: collect class to detail customizations map
	// NEEDN'T: build detail call chain(child first) 
	// in this function just do these 

	// here begin detail customization
	// step1: find lowest class 
	// step2: search detail call chain 
	// step3: loop call and decode for hide/show parent

	// collect classes to detail customization map
	for (TObjectIterator<UClass> ItClass; ItClass; ++ItClass)
	{
		UClass* TargetClass = *ItClass;
		
		// reroute 
		if (ItClass->HasMetaData(TEXT("QEReroute")))
		{
			TargetClass = FindObjectChecked<UClass>(ANY_PACKAGE, *ItClass->GetMetaData(TEXT("QEReroute")));
		}

		// each function to find customization function 
		for (TFieldIterator<UFunction> ItFunc(*ItClass); ItFunc; ++ItFunc)
		{
			if (ItFunc->HasMetaData(TEXT("QEDetail")))
			{
				CHECK_STATIC(ItFunc);
				DetailCustomizationMap.Add(TargetClass, *ItFunc);
			}
			else if (ItFunc->HasMetaData(TEXT("QEItemDetail")))
			{
				CHECK_STATIC(ItFunc);
				PropertyCustomizationMap.Add(TargetClass, *ItFunc);
			}
		}
	}
}

#undef CHECK_STATIC
#undef COND_ERR
