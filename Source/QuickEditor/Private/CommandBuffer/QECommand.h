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
};

struct FQECommand
{
	EQECommand	CommandType;

	virtual ~FQECommand(){}
};

struct FQEAddEntry : public FQECommand
{
	static constexpr auto Type = EQECommand::Menu_AddEntry;
	FString		EntryName;
	FString		EntryIcon;
};

struct FQEWidget : public FQECommand
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

