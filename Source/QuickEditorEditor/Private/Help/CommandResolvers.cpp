#include "CommandResolvers.h"

#include "ContentBrowserModule.h"
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
