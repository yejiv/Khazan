#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CDebug_Controller final : public CBase
{
private:
	CDebug_Controller();
	virtual ~CDebug_Controller() = default;

public:
	HRESULT Initialize();
	void Update();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	_float4 m_vRenderTargetValue = {};

	//vector<_wstring> m_vRenderTarget;
	_char m_szSelectRenderTarget[MAX_PATH];

public:
	void Ready_ImGui();

public:
	static CDebug_Controller* Create();
	virtual void Free() override;
};

NS_END