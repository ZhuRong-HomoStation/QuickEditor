#pragma once
#include "QECommand.h"

class QUICKEDITOR_API FQECmdBuffer
{
public:
	void Reset()
	{
		m_Buffer.Reset();
		m_Idx = 0;
	}

	template<typename TCmd>
	void Write(TCmd& InCmd)
	{
		static_assert(TIsDerivedFrom<TCmd, FQECommand>::Value, "Command must derived from base command");

		InCmd.CommandType = TCmd::Type;
		uint32 Idx = m_Buffer.AddUninitialized(sizeof(TCmd));
		TCmd* Cmd = reinterpret_cast<TCmd*>(m_Buffer.GetData() + Idx);
		new (Cmd)TCmd(InCmd);
	}

	EQECommand Peek()
	{
		if (m_Idx >= m_Buffer.Num()) return EQECommand::Unknown;
		return reinterpret_cast<FQECommand*>(m_Buffer.GetData() + m_Idx)->CommandType;
	}

	template<typename TCmd>
	TCmd Read()
	{
		static_assert(TIsDerivedFrom<TCmd, FQECommand>::Value, "Command must derived from base command");

		check(TCmd::Type == Peek());
		check(m_Idx + sizeof(TCmd) <= m_Buffer.Num());

		TCmd* Cmd = reinterpret_cast<TCmd*>(m_Buffer.GetData() + m_Idx);
		m_Idx += sizeof(TCmd);
		return MoveTemp(*Cmd);
	}

private:
	TArray<uint8>			m_Buffer;
	int32					m_Idx = 0;
};
