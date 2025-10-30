#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CDebug_Manager final : public CBase
{
private:
	CDebug_Manager();
	virtual ~CDebug_Manager() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

private:
	void Ready_FPS();
	void Ready_CameraDebug();
	void Ready_JoltDebug();
	
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CClientInstance* m_pClientInstance = { nullptr };

	// Jolt
	_bool m_isCollisionRender[ENUM_CLASS(COLLISION_LAYER::END)];

	_char					m_szFPS[MAX_PATH] = {};
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {};
public:
	static CDebug_Manager* Create();
	virtual void Free() override;
};

NS_END