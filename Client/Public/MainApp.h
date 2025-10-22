#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "ClientInstance.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CClientInstance*		m_pClientInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

#ifdef _DEBUG
	_tchar					m_szFPS[MAX_PATH] = {};
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {}; 
#endif

private:
	HRESULT Ready_Prototype_ForStatic();
	HRESULT Ready_Prototype_ForStatic_UI();
	HRESULT Ready_DB();
	HRESULT Ready_ObjectLayer();
	HRESULT Ready_ClientInstance(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	HRESULT Start_Level(LEVEL eStartLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;		
};

NS_END


