#include "CommandResolvers.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "QuickEditor_Internal.h"
#include "CommandBuffer/QECmdBuffer.h"
#include "CommandBuffer/QECommand.h"
#include "Engine/Selection.h"

class FContentBrowserModule;

void QEPrivate::ResolveMenuCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

	while (Cmd != EQECommand::Unknown)
	{
		switch (Cmd)
		{
		case EQECommand::Menu_AddEntry:		_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddEntry>()); break;
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>()); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>()); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>());break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
	}
}

void QEPrivate::ResolveActorCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

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
			_ResolveMenuCommand(InBuilder, AddEntry);
			break;
		}
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>()); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>()); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>());break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
	}
}

void QEPrivate::ResolveAssetCommands(FMenuBuilder& InBuilder)
{
	FQECmdBuffer& Buffer = QE::GetCmdBuffer();

	EQECommand Cmd = Buffer.Peek();

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
			_ResolveMenuCommand(InBuilder, AddEntry);
			break;
		}
		case EQECommand::Menu_AddWidget:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEAddWidget>()); break;
		case EQECommand::Menu_BeginSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEBeginSection>()); break;
		case EQECommand::Menu_EndSection:	_ResolveMenuCommand(InBuilder, Buffer.Read<FQEEndSection>());break;
		default: checkNoEntry();
		}
		Cmd = Buffer.Peek();
	}
}

void QEPrivate::_ResolveMenuCommand(FMenuBuilder& InBuilder, const FQECommand& InCmd)
{
	switch (InCmd.CommandType)
	{
	case EQECommand::Menu_AddEntry:
	{
		const FQEAddEntry& Cmd = static_cast<const FQEAddEntry&>(InCmd);

		FString StyleSet, StyleName;
		Cmd.EntryIcon.Split(TEXT("::"), &StyleSet, &StyleName);

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
