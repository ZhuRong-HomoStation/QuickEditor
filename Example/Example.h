#pragma once
#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Example.generated.h"

UCLASS()
class UMenuExample : public UObject
{
	GENERATED_BODY()
public:
	// Example1: No Item menu 
	UFUNCTION(meta=(QEMenuItem="QEExample/Simple/Example1"))
    static void Example1(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example1")); }

	// Example2: Item with Icon(Path)
	UFUNCTION(meta=(QEMenuItem="QEExample/Simple/Example2", QEIcon="QuickEditor/Resources/Icon128.png"))
    static void Example2(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example2")); }

	// Example3: Item with Icon(StyleSet)
	UFUNCTION(meta=(QEMenuItem="QEExample/Simple/Example3", QEIcon="EditorStyle::GraphEditor.Enum_16x"))
    static void Example3(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example3")); }

	// Example4: Item with Section (Path)
	UFUNCTION(meta=(QEMenuItem="QEExample/Cate1.Section/Example4"))
    static void Example4(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example4")); }

	// Example5: Item with Section (Entry)
	UFUNCTION(meta=(QEMenuItem="QEExample/Cate1.Section/CateA.Example5"))
    static void Example5(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example5")); }

	// Example6: Item with Section (Entry)
	UFUNCTION(meta=(QEMenuItem="QEExample/Cate1.Section/CateB.Example6"))
    static void Example6(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example6")); }

	// Example7: Custom pop up 
	UFUNCTION(meta=(QEMenuItem="QEExample/Cate1.Section/CateC.Example7", QEPopUp))
	static void Example7();
};

UCLASS()
class UToolBarExample : public UObject
{
	GENERATED_BODY()
public:
	// Example1: Toolbar button 
	UFUNCTION(meta=(QEToolBar="Button", QEIcon="QuickEditor/Resources/Icon128.png", QEIconSize="50, 50"))
    static void Example1(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example1")); }

	// Example2: Toolbar Popup 
	UFUNCTION(meta=(QEToolBar="PopUp/Cate1.Item", QEIcon="QuickEditor/Resources/Icon128.png"))
    static void Example2(){ UKismetSystemLibrary::PrintString(GWorld, TEXT("Example2")); }

	// Example3: Custom Popup
	UFUNCTION(meta=(QEToolBar="PopUp/Cate2.PopUp", QEIcon="QuickEditor/Resources/Icon128.png", QEPopUp))
    static void Example3(){ UMenuExample::Example7(); }

	// Menu usage same as menu example 
};

// reroute actions to class Actor 
UCLASS(meta=(QEReroute="Actor"))
class UActorExample : public UObject
{
	GENERATED_BODY()
public:
	// Example1: Actor action
	UFUNCTION(meta=(QEActorItem="MyAction.Example1", QEIcon="QuickEditor/Resources/Icon128.png"))
	static void Example1();

	// Example2: Custom PopUp
	UFUNCTION(meta=(QEActorItem="MyAction.Example2", QEIcon="QuickEditor/Resources/Icon128.png", QEPopUp))
	static void Example2();

	// Menu usage same as menu example 
};

// reroute actions to class Object
UCLASS(meta=(QEReroute="Object"))
class UAssetExample : public UObject
{
	GENERATED_BODY()
public:
	// Example1: Asset action
	UFUNCTION(meta=(QEAssetItem="MyAction.Example1", QEIcon="QuickEditor/Resources/Icon128.png"))
	static void Example1();

	// Example2: Custom PopUp
	UFUNCTION(meta=(QEAssetItem="MyAction.Example2", QEIcon="QuickEditor/Resources/Icon128.png", QEPopUp))
	static void Example2();

	// Menu usage same as menu example 
};

// Text asset
UCLASS()
class UTextAsset : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString		Text;
};

// Factories
UCLASS(meta=(QEReroute="TextAsset"))
class UNewAssetExample : public UObject
{
	GENERATED_BODY()
public:
	// Example1: New asset
	UFUNCTION(meta=(QECreateNew="MyAsset::Text", QEIcon="QuickEditor/Resources/Icon128.png", QEIconSize="60, 60"))
	static void NewAsset();

	// Example2: New asset from file
	UFUNCTION(meta=(QECreateFile="txt"))
	static void ImportAsset();
};
