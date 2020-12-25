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
		FDetailCmdResolver(IDetailLayoutBuilder* InDetailBuilder);
		void Reset();
		void ResolveCommands();
	
	public:
		IDetailLayoutBuilder*		DetailBuilder;
		IDetailCategoryBuilder*		CurCategoryBuilder;
	};

	void _ResolveMenuCommand(FMenuBuilder& InBuilder, const FQECommand& InCmd, int32 CommandID);
}
