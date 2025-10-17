#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "Effect_Prefab.h"

NS_BEGIN(Editor)

class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	class CEffect_Prefab* m_PrefabPrototype;

private:

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_GameObejct();

public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END