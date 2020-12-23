#pragma once

class FMenuBuilder;
struct FQECommand;

namespace QEPrivate
{
	void ResolveMenuCommands(FMenuBuilder& InBuilder);
	void ResolveActorCommands(FMenuBuilder& InBuilder);
	void ResolveAssetCommands(FMenuBuilder& InBuilder);

	void _ResolveMenuCommand(FMenuBuilder& InBuilder, const FQECommand& InCmd, int32 CommandID);
}
