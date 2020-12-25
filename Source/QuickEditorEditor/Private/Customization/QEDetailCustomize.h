#pragma once
#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"
#include "CommandBuffer/QECmdBuffer.h"

class FQEDetailCustomizePerClass : public IDetailCustomization
{
public:
	FQEDetailCustomizePerClass(UClass* InTargetClass);
	static TSharedRef<IDetailCustomization> MakeInstance(UClass* TargetClass);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	UClass*				TargetClass;
};

class FQEDetailCustomize : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(
		TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(
		TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	static TArray<UClass*>		DoneDetails;
};

class FQEPropertyCustomize : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(
        TSharedRef<IPropertyHandle> PropertyHandle,
        FDetailWidgetRow& HeaderRow,
        IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(
        TSharedRef<IPropertyHandle> PropertyHandle,
        IDetailChildrenBuilder& ChildBuilder,
        IPropertyTypeCustomizationUtils& CustomizationUtils) override;
};
