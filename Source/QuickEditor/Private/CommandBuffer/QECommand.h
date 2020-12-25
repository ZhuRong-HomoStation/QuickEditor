#pragma once
#include "CoreMinimal.h"

enum class EQECommand
{
	Unknown ,
	
	Menu_AddEntry ,
	Menu_AddWidget ,
	Menu_BeginSection ,
	Menu_EndSection ,

	Detail_HideParent ,
	Detail_ShowParent ,
	Detail_HideCategory ,
	Detail_EditCategory ,
	Detail_HideProperty ,
	Detail_EditProperty ,
	Detail_AddWidget ,
};

struct FQECommand
{
	EQECommand	CommandType;

	virtual ~FQECommand(){}
};

struct FQEAddEntry : public FQECommand
{
	static constexpr auto Type = EQECommand::Menu_AddEntry;
	FString			EntryName;
	FString			EntryIcon;
	FSimpleDelegate EntryEvent;
};

struct FQEAddWidget : public FQECommand
{
	static constexpr auto Type = EQECommand::Menu_AddWidget;
	TSharedPtr<SWidget>		Widget;
};

struct FQEBeginSection : public FQECommand
{
	static constexpr auto Type = EQECommand::Menu_BeginSection;
	FString		SectionName;
};

struct FQEEndSection : public FQECommand
{
	static constexpr auto Type = EQECommand::Menu_EndSection;
};

struct FQEHideParent : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_HideParent;
	FString		ParentName;
};

struct FQEShowParent : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_ShowParent;
	FString		ParentName;
};

struct FQEHideCategory : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_HideCategory;
	FString		Category;
};

struct FQEEditCategory : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_EditCategory;
	FString		Category;
	int32		Priority;
};

struct FQEHideProperty : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_HideProperty;
	FString		PropertyName;
};

struct FQEEditProperty : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_EditProperty;
	FString					PropertyName;
	FString					OverrideName;
	TSharedPtr<SWidget>		NameWidget;
	TSharedPtr<SWidget>		ValueWidget;
};

struct FQEDetailAddWidget : public FQECommand
{
	static constexpr auto Type = EQECommand::Detail_AddWidget;
	FString					WidgetName;
	FString					SearchName;
	TSharedPtr<SWidget>		NameWidget;
	TSharedPtr<SWidget>		ValueWidget;
};


