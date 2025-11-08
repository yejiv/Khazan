#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CPlayer_Manager final : public CBase
{
private:
	CPlayer_Manager();
	virtual ~CPlayer_Manager() = default;

public:
	const PLAYER_DATA&			Get_PlayerData() { return m_Data; }
	void						Add_SkillExp(_float fExp);
	_bool						Add_SkillPoint(_int iPoint);

public:
	HRESULT						Initialize();
private:
	PLAYER_DATA					m_Data = {};

public:
	static CPlayer_Manager*		Create();
	virtual void				Free() override;
};

NS_END