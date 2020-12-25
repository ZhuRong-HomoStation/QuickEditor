#include "Example.h"
#include "Misc/FileHelper.h"
#include "QuickEditor.h"
#include "Engine/StaticMeshActor.h"
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

void UMenuExample::Example8()
{
	QE::Window::WndName(TEXT("臭臭臭"));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
	QE::Window::WndSize(FVector2D(600, 600));
}

void UMenuExample::Example9()
{
	QE::Window::WndName(TEXT("臭臭臭"));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
}

void UMenuExample::Example10()
{
	QE::Window::WndName(TEXT("臭臭臭"));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
	QE::Window::WndSize(FVector2D(600, 600));
}

void UMenuExample::Example11()
{
	QE::Window::WndName(TEXT("臭臭臭_") + FString::FromInt(QE::Window::GetID()));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
	QE::Window::WndSize(FVector2D(600, 600));
}

void UMenuExample::Example12()
{
	QE::Window::WndName(TEXT("臭臭臭_") + FString::FromInt(QE::Window::GetID()));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
}

void UToolBarExample::Example1()
{
	QE::Window::WndName(TEXT("臭臭臭"));
	QE::Window::WndContent(SNew(SImage)
                    .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                        TEXT("MenuExample.Example2"))));
	QE::Window::WndSize(FVector2D(600, 600));
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

	// use QE::AssetNew::IsReimport() control logic when import or reimport
	// QE::AssetNew::CreateDefault() will process this case automatic 
}

AExampleActor::AExampleActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void UCustomDetailExampleA::CustomDetailExample()
{
	decltype(auto) Objs = QE::Detail::GetSelectedObjects();

	// default category is "Default"
	for (UObject* Obj : Objs)
	{
		QE::Detail::AddWidget(Obj->GetName(), SNew(STextBlock).Text(FText::FromString(Obj->GetName())));
	}

	// hide category
	QE::Detail::HideCategory(TEXT("Actor"));
	
	// edit category
	QE::Detail::EditCategory(TEXT("臭臭臭"), 0);

	// widget we will use 
	TSharedRef<SWidget> SmellyMan =
		SNew(SBox)
		.WidthOverride(200)
		.HeightOverride(200)
		[
			SNew(SImage)
	        .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
	                                                       TEXT("MenuExample.Example2")))
		];

	// add whole row widget
	QE::Detail::AddWidget(TEXT("臭臭先生A"), SmellyMan);

	// add value widget
	QE::Detail::AddWidget(TEXT("臭臭先生B"), SmellyMan, TEXT("臭臭先生B"));

	// add custom name widget
	QE::Detail::AddWidget(TEXT("臭臭先生C"), SmellyMan, SmellyMan);
}

void UExampleActorDetail::CustomDetailExample()
{
	// widget we will use 
	TSharedRef<SWidget> SmellyMan =
        SNew(SBox)
        .WidthOverride(200)
        .HeightOverride(200)
        [
            SNew(SImage)
            .Image(FSlateStyleRegistry::FindSlateStyle(TEXT("QEStyleSet"))->GetBrush(
                                                           TEXT("MenuExample.Example2")))
        ];
	
	// Hide parent detail customize 
	QE::Detail::HideParent(TEXT("Object"));

	// !!!!!don't use functions about properties, they are invalid for a while  

	// Hide Property
	QE::Detail::HideProperty(TEXT("ExampleActor.PropertyA"));

	// Edit Property whole row 
	QE::Detail::EditProperty(TEXT("ExampleActor.PropertyB"), SmellyMan);

	// Edit Property with name  
	QE::Detail::EditProperty(TEXT("ExampleActor.PropertyC"), SmellyMan, TEXT("CCCCCCC"));

	// Edit Property with custom name widget
	QE::Detail::EditProperty(TEXT("ExampleActor.PropertyC"), SmellyMan, SNew(SButton)[SmellyMan]);
}

void AExampleActorChild::CustomDetailExample()
{
	// Show parent(witch we hide it in AExampleActor)
	QE::Detail::ShowParent(TEXT("Object"));
}
