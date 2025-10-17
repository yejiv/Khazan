#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Shader final : public CLevel
{
private:
	CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Shader() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();

#ifdef _DEBUG
private:
	// Frame Check
	_tchar					m_szFPS[MAX_PATH] = {};
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {};
#endif

public:
	static CLevel_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END