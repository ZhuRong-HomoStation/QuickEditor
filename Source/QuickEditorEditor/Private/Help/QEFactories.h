﻿#pragma once
#include "CoreMinimal.h"

#include "EditorReimportHandler.h"

#include "QEFactories.generated.h"

UCLASS(CustomConstructor)
class UQEFactoryNew : public UFactory
{
	GENERATED_BODY()
public:
	UQEFactoryNew(const FObjectInitializer& ObjectInitializer);
protected:
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
public:
	static TMap<UClass*, UFunction*> NewFunctionMap;
	static TMap<UClass*, UClass*> SupportClassMap;
};

UCLASS(CustomConstructor)
class UQEFactoryFile : public UFactory, public FReimportHandler
{
	GENERATED_BODY()
public:
	UQEFactoryFile(const FObjectInitializer& ObjectInitializer);
public:
	virtual void GetSupportedFileExtensions(TArray<FString>& OutExtensions) const override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	// ~Begin FReimportHandler API 
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	// ~End FReimportHandler API 
public:
	static TMap<UClass*, UFunction*> NewFunctionMap;
	static TMap<UClass*, UClass*> SupportClassMap;
};