#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "UIObject.h"


NS_BEGIN(Editor)

class CLevel_UI final : public CLevel
{
private:
	CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_UI() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT				Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT				Ready_Obejct();

private:
	class CEdit_Interface_UI* m_pUIInterface = { nullptr };
	_float						m_fTimeDelta = {};

private:
	void				Update_Interface();
public:
	static CLevel_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END