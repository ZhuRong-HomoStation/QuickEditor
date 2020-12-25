#pragma once
#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"
#include "CommandBuffer/QECmdBuffer.h"

class FQEDetailCustomizePerClass : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance(UClass* TargetClass);

	FQEDetailCustomizePerClass(UClass* InTargetClass);

protected:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
public:
	UClass*				TargetClass;
};

class FQEDetailCustomize : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	virtual void CustomizeHeader(
		TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(
		TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;

public:
	static TArray<UClass*>		DoneDetails;
};

class FQEPropertyCustomize : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(UClass* TargetClass);

	FQEPropertyCustomize(UClass* InTargetClass);

protected:
	virtual void CustomizeHeader(
        TSharedRef<IPropertyHandle> PropertyHandle,
        FDetailWidgetRow& HeaderRow,
        IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(
        TSharedRef<IPropertyHandle> PropertyHandle,
        IDetailChildrenBuilder& ChildBuilder,
        IPropertyTypeCustomizationUtils& CustomizationUtils) override;
public:
	UClass*			TargetClass;
};
