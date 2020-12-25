#pragma once

class IDetailCategoryBuilder;
class IDetailLayoutBuilder;
class FMenuBuilder;
struct FQECommand;

namespace QEPrivate
{
	void ResolveMenuCommands(FMenuBuilder& InBuilder);
	void ResolveActorCommands(FMenuBuilder& InBuilder);
	void ResolveAssetCommands(FMenuBuilder& InBuilder);

	class FDetailCmdResolver
	{
	public:
		FDetailCmdResolver(IDetailLayoutBuilder* InDetailBuilder, IPropertyTypeCustomizationUtils* InCustomizationUtils);
		void Reset();
		void CleanUp();
		void ResolveCommands();
		bool QueryShowState(const FString& ClassName);
	
	private:
		IDetailLayoutBuilder*	DetailBuilder;
		IPropertyTypeCustomizationUtils*	CustomizationUtils;
		IDetailCategoryBuilder*	CurCategoryBuilder;
		TMap<FString, bool>		OverrideStateMap;
	};

	void _ResolveMenuCommand(FMenuBuilder& InBuilder, const FQECommand& InCmd, int32 CommandID);
}
