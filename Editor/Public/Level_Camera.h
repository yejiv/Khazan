#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Camera final : public CLevel
{
private:
	CLevel_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Camera() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

public:
	static CLevel_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END