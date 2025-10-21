#pragma once

#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

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
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CDebug_Controller* m_pDebug = { nullptr };

#ifdef _DEBUG
	_tchar					m_szFPS[MAX_PATH] = {};
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {};
#endif

private:
	HRESULT Ready_Prototype_ForStatic();
	HRESULT Ready_ObjectLayer();
	HRESULT Start_Level(LEVEL eStartLevelID);

private:
	void Ready_DefaultImgui();
	void Ready_Debug();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

NS_END


