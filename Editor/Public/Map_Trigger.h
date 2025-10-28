#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"



NS_BEGIN(Editor)

class CMap_Trigger final : public CGameObject
{
private:
	CMap_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap_Trigger(const CMap_Trigger& Prototype);
	virtual ~CMap_Trigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMap_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END