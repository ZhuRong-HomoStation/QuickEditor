#pragma once
#include "AssetTypeActions_Base.h"

class FQEAssetTypeAction : public FAssetTypeActions_Base
{
	using Super = FAssetTypeActions_Base;
public:
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return AssetCategory; }
	virtual FText GetName() const override { return FText::FromString(Name); }
	virtual UClass* GetSupportedClass() const override { return SupportClass; }
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool IsImportedAsset() const override { return bIsImportedAsset; }
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
		TArray<FString>& OutSourceFilePaths) const override;
	
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;

public:
	EAssetTypeCategories::Type		AssetCategory;
	UClass*							SupportClass;
	FString							Name;
	bool							bIsImportedAsset;
};
