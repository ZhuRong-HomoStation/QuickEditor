#include "Example.h"
#include "Misc/FileHelper.h"
#include "QuickEditor.h"
#include "Styling/SlateStyleRegistry.h"

void UMenuExample::Example7()
{
	QE::Menu::BeginSection(TEXT("Entries"));
	for (int i = 0; i < 3; ++i)
	{
		FString Name(TEXT("Entry"));
		Name.AppendChar(TEXT('A') + i);
		QE::Menu::AddEntry(
			Name,
			TEXT("QuickEditor/Resources/Icon128.png"),
			FSimpleDelegate::CreateLambda([Name]
			{
				UKismetSystemLibrary::PrintString(GWorld, Name);
			}));
	}
	QE::Menu::EndSection();

	QE::Menu::BeginSection(TEXT("Buttons"));
	for (int i = 0; i < 3; ++i)
	{
		FString Name(TEXT("Button"));
		Name.AppendChar(TEXT('A') + i);
		QE::Menu::AddWidget(
			SNew(SButton)
            .OnClicked_Lambda([Name]
			 {
            	FSlateApplication::Get().AddModalWindow(SNew(SWindow)
            	.SizingRule(ESizingRule::FixedSize)
            	.Title(FText::FromString(TEXT("臭臭臭")))
            	.ClientSize(FVector2D(600, 600))
                [
                    SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2")))
                ], nullptr);
            	
				 return FReply::Handled();
			 })
			 [
				SNew(SImage)
				.Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
					TEXT("MenuExample.Example2")))
			 ]);
	}
	QE::Menu::EndSection();
}

void UActorExample::Example1()
{
	const TArray<AActor*>& SelectedActors = QE::Actor::GetSelectedActor();
	for (AActor* Actor : SelectedActors)
	{
		UKismetSystemLibrary::PrintString(GWorld, Actor->GetPathName());
	}
}

void UActorExample::Example2()
{
	const TArray<AActor*>& SelectedActors = QE::Actor::GetSelectedActor();

	QE::Menu::BeginSection(TEXT("Entries"));
	for (AActor* Actor : SelectedActors)
	{
		QE::Menu::AddEntry(
			Actor->GetActorLabel(),
			TEXT("EditorStyle::GraphEditor.Enum_16x"),
			FSimpleDelegate::CreateLambda([Actor]
			{
				UKismetSystemLibrary::PrintString(GWorld, Actor->GetActorLabel());
			}));
	}
	QE::Menu::EndSection();

	QE::Menu::BeginSection(TEXT("Buttons"));
	for (AActor* Actor : SelectedActors)
	{
		QE::Menu::AddWidget(
			SNew(SButton)
                .Text(FText::FromString(Actor->GetActorLabel()))
                .OnClicked_Lambda([Actor]
						 {
							 UKismetSystemLibrary::PrintString(GWorld, Actor->GetActorLabel());
							 return FReply::Handled();
						 }));
	}
	QE::Menu::EndSection();
}

void UAssetExample::Example1()
{
	const TArray<UObject*>& SelectedAssets = QE::Asset::GetSelectedAsset();
	for (UObject* Asset : SelectedAssets)
	{
		UKismetSystemLibrary::PrintString(GWorld, Asset->GetPathName());
	}
}

void UAssetExample::Example2()
{
	const TArray<UObject*>& SelectedAssets = QE::Asset::GetSelectedAsset();

	QE::Menu::BeginSection(TEXT("Entries"));
	for (UObject* Asset : SelectedAssets)
	{
		QE::Menu::AddEntry(
			Asset->GetName(),
			TEXT("EditorStyle::GraphEditor.Enum_16x"),
			FSimpleDelegate::CreateLambda([Asset]
			{
				UKismetSystemLibrary::PrintString(GWorld, Asset->GetName());
			}));
	}
	QE::Menu::EndSection();

	QE::Menu::BeginSection(TEXT("Buttons"));
	for (UObject* Asset : SelectedAssets)
	{
		QE::Menu::AddWidget(
			SNew(SButton)
                .Text(FText::FromString(Asset->GetName()))
                .OnClicked_Lambda([Asset]
						 {
							 UKismetSystemLibrary::PrintString(GWorld, Asset->GetName());
							 return FReply::Handled();
						 }));
	}
	QE::Menu::EndSection();
}

void UNewAssetExample::NewAsset()
{
	QE::AssetNew::CreateDefault();

	// QE::AssetNew::CreateDefault() same as
	// 
	// UTextAsset* NewAsset = NewObject<UTextAsset>(
	// 	QE::AssetNew::GetOuter(),
	// 	QE::AssetNew::GetClass(),
	// 	QE::AssetNew::GetName(),
	// 	QE::AssetNew::GetFlags());
	// QE::AssetNew::CreatedObject(NewAsset);
}

void UNewAssetExample::ImportAsset()
{
	UTextAsset* Asset = (UTextAsset*)QE::AssetNew::CreateDefault();
	FFileHelper::LoadFileToString(Asset->Text, *QE::AssetNew::GetImportFilePath());
}
