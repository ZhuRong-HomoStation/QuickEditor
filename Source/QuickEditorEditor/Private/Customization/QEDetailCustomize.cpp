#include "QEDetailCustomize.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "Logging.h"
#include "PropertyCustomizationHelpers.h"

TSharedRef<IPropertyTypeCustomization> FQEDetailCustomize::MakeInstance()
{
	return MakeShareable(new FQEDetailCustomize());
}

void FQEDetailCustomize::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FQEDetailCustomize::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// init info 
	IDetailLayoutBuilder* DetailBuilder = &ChildBuilder.GetParentCategory().GetParentLayout();
	TArray<UObject*> AllObjects;
	PropertyHandle->GetOuterObjects(AllObjects);

	// hide self
	DetailBuilder->HideCategory(TEXT("QuickEditor"));

	
	// auto CurCate = &DetailBuilder->EditCategory(TEXT("Actor"), FText::FromString(TEXT("Actor")));
	// TSharedRef<IPropertyHandle> Test = DetailBuilder->GetProperty(GET_MEMBER_NAME_CHECKED(AActor, Tags), AActor::StaticClass());
	// Test->GetChildHandle(0);
	

}
