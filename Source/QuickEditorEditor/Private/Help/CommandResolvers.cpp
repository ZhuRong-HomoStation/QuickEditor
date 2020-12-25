#include "CommandResolvers.h"

#include "ContentBrowserModule.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IContentBrowserSingleton.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"
#include "CommandBuffer/QECommand.h"
#include "Engine/Selection.h"
#include "Services/QuickEditorService.h"

class FContentBrowserModule;

void QEPrivate::ResolveMenuCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

	int32 Count = 0;
	while (Cmd != EQECommand::Unknown)
	{
		switch (Cmd)
		{
		case EQECommand::Menu_AddEntry:		_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddEntry>(), Count); break;
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>(), Count); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>(), Count); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>(), Count);break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
		++Count;
	}
}

void QEPrivate::ResolveActorCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

	int32 Count = 0;
	while (Cmd != EQECommand::Unknown)
	{
		switch (Cmd)
		{
		case EQECommand::Menu_AddEntry:
		{
			FQEAddEntry AddEntry = Buffer.Read<FQEAddEntry>();
			AddEntry.EntryEvent = FSimpleDelegate::CreateLambda([Event=AddEntry.EntryEvent]
            {
                GEditor->GetSelectedActors()->GetSelectedObjects(QE::SelectedActors);
				QE::Actor::ActorState(true);

				Event.ExecuteIfBound();

				QE::Actor::ActorState(false);
				QE::SelectedActors.Reset();
            });
			_ResolveMenuCommand(InBuilder, AddEntry, Count);
			break;
		}
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>(), Count); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>(), Count); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>(), Count);break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
		++Count;
	}
}

void QEPrivate::ResolveAssetCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

	int32 Count = 0;
	while (Cmd != EQECommand::Unknown)
	{
		switch (Cmd)
		{
		case EQECommand::Menu_AddEntry:
		{
			FQEAddEntry AddEntry = Buffer.Read<FQEAddEntry>();
			AddEntry.EntryEvent = FSimpleDelegate::CreateLambda([Event=AddEntry.EntryEvent]
			{
				FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
			    TArray<FAssetData> SelectedAssets;
			    ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
			    for (FAssetData& Asset : SelectedAssets)
			    {
			        QE::SelectedAssets.Add(Asset.GetAsset());
			    }
			    QE::Asset::AssetState(true);

				Event.ExecuteIfBound();
				
				QE::SelectedAssets.Reset();
				QE::Asset::AssetState(false);
			});
			_ResolveMenuCommand(InBuilder, AddEntry, Count);
			break;
		}
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>(), Count); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>(), Count); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>(), Count);break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
		++Count;
	}
}

QEPrivate::FDetailCmdResolver::FDetailCmdResolver(IDetailLayoutBuilder* InDetailBuilder, IPropertyTypeCustomizationUtils* InCustomizationUtils)
	: DetailBuilder(InDetailBuilder)
	, CustomizationUtils(InCustomizationUtils)
{
	check(CurCategoryBuilder != nullptr);
	CurCategoryBuilder = &DetailBuilder->EditCategory(TEXT("Default"));
}

void QEPrivate::FDetailCmdResolver::Reset()
{
	CurCategoryBuilder = &DetailBuilder->EditCategory(TEXT("Default"));
}

void QEPrivate::FDetailCmdResolver::CleanUp()
{
	OverrideStateMap.Reset();
	CurCategoryBuilder = &DetailBuilder->EditCategory(TEXT("Default"));	
}

void QEPrivate::FDetailCmdResolver::ResolveCommands()
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand CmdType = Buffer.Peek();
	while (CmdType != EQECommand::Unknown)
	{
		switch (CmdType)
		{
		case EQECommand::Detail_HideParent:
		{
			auto Cmd = Buffer.Read<FQEHideParent>();
			if (!OverrideStateMap.Contains(Cmd.ParentName))
			{
				OverrideStateMap.Add(Cmd.ParentName, false);
			}
			break;
		}
		case EQECommand::Detail_ShowParent:
		{
			auto Cmd = Buffer.Read<FQEShowParent>();
			if (!OverrideStateMap.Contains(Cmd.ParentName))
			{
				OverrideStateMap.Add(Cmd.ParentName, true);
			}
			break;
		}
		case EQECommand::Detail_HideCategory:
		{
			auto Cmd = Buffer.Read<FQEHideCategory>();
			DetailBuilder->HideCategory(FName(Cmd.Category));
			break;
		}
		case EQECommand::Detail_EditCategory:
		{
			auto Cmd = Buffer.Read<FQEEditCategory>();
			CurCategoryBuilder = &DetailBuilder->EditCategory(FName(Cmd.Category), FText::FromString(Cmd.Category), (ECategoryPriority::Type)(Cmd.Priority));
			break;
		}
		case EQECommand::Detail_HideProperty:
		{
			auto Cmd = Buffer.Read<FQEHideProperty>();
			break;
			FString PropertyClass, PropertyName;
			Cmd.PropertyName.Split(TEXT("."), &PropertyClass, &PropertyName);
			DetailBuilder->HideProperty(FName(Cmd.PropertyName), FindObjectChecked<UStruct>(ANY_PACKAGE, *PropertyClass));
			break;
		}
		case EQECommand::Detail_EditProperty:
		{
			auto Cmd = Buffer.Read<FQEEditProperty>();
			break;
			FString PropertyClass, PropertyName;
			Cmd.PropertyName.Split(TEXT("."), &PropertyClass, &PropertyName);
			auto Property = DetailBuilder->GetProperty(FName(PropertyName), FindObjectChecked<UStruct>(ANY_PACKAGE, *PropertyClass));
			auto& PropertyRow = CurCategoryBuilder->AddProperty(Property);
			if (!Cmd.OverrideName.IsEmpty())
			{
				auto& CustomRow = PropertyRow.CustomWidget();
				CustomRow.NameContent()
                [
                    Cmd.NameWidget.ToSharedRef()
                ];
				CustomRow.ValueContent()
                [
                    Cmd.ValueWidget.ToSharedRef()
                ];
			}
			else if (Cmd.NameWidget.IsValid())
			{
				auto& CustomRow = PropertyRow.CustomWidget();
				CustomRow.NameContent()
				[
					SNew(STextBlock)
		            .Text(FText::FromString(Cmd.OverrideName))
		            .Font(CustomizationUtils->GetRegularFont())
				];
				CustomRow.ValueContent()
				[
					Cmd.ValueWidget.ToSharedRef()
				];
			}
			else
			{
				PropertyRow.CustomWidget()
                .WholeRowContent()
                [
                    Cmd.ValueWidget.ToSharedRef()
                ];
			}
			break;
		}
		case EQECommand::Detail_AddWidget:
		{
			auto Cmd = Buffer.Read<FQEDetailAddWidget>();
			auto& CustomRow = CurCategoryBuilder->AddCustomRow(FText::FromString(Cmd.SearchName));
			if (!Cmd.WidgetName.IsEmpty())
			{
				CustomRow.NameContent()
				[
					SNew(STextBlock)
	                .Text(FText::FromString(Cmd.WidgetName))
	                .Font(CustomizationUtils->GetRegularFont())
				];
				CustomRow.ValueContent()
				[
					Cmd.ValueWidget.ToSharedRef()
				];
			}
			else if (Cmd.NameWidget.IsValid())
			{
				CustomRow.NameContent()
                [
                    Cmd.NameWidget.ToSharedRef()
                ];
				CustomRow.ValueContent()
                [
                    Cmd.ValueWidget.ToSharedRef()
                ];
			}
			else
			{
				CustomRow.WholeRowContent()
                [
                    Cmd.ValueWidget.ToSharedRef()
                ];
			}
			break;
		}
		default: checkNoEntry();
		}
		CmdType = Buffer.Peek();
	}

}

bool QEPrivate::FDetailCmdResolver::QueryShowState(const FString& ClassName)
{
	bool* FoundState = OverrideStateMap.Find(ClassName);
	return FoundState ? *FoundState : true;
}

void QEPrivate::_ResolveMenuCommand(FMenuBuilder& InBuilder, const FQECommand& InCmd, int32 CommandID)
{
	switch (InCmd.CommandType)
	{
	case EQECommand::Menu_AddEntry:
	{
		const FQEAddEntry& Cmd = static_cast<const FQEAddEntry&>(InCmd);

		FString StyleSet, StyleName;
		if (!Cmd.EntryIcon.Split(TEXT("::"), &StyleSet, &StyleName))
		{
			StyleSet = TEXT("QEStyleSet");
			StyleName = TEXT("Entry.") + Cmd.EntryIcon;
			UQuickEditorService::Get().AddIcon(StyleName, Cmd.EntryIcon);
		}

		InBuilder.AddMenuEntry(
			FText::FromString(Cmd.EntryName),
			FText::GetEmpty(),
			FSlateIcon(*StyleSet, *StyleName),
			FUIAction(Cmd.EntryEvent));
		break;
	}
	case EQECommand::Menu_AddWidget:
	{
		const FQEAddWidget& Cmd = static_cast<const FQEAddWidget&>(InCmd);

		InBuilder.AddWidget(Cmd.Widget.ToSharedRef(), FText::GetEmpty());
		break;
	}
	case EQECommand::Menu_BeginSection:
	{
		const FQEBeginSection& Cmd = static_cast<const FQEBeginSection&>(InCmd);

		InBuilder.BeginSection(*Cmd.SectionName, FText::FromString(Cmd.SectionName));
		break;
	}
	case EQECommand::Menu_EndSection:
	{
		InBuilder.EndSection();
		break;
	}
	default: checkNoEntry();
	}
}
