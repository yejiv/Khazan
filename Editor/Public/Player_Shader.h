#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CPlayer_Shader final : public CGameObject
{
private:
	CPlayer_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Shader(const CPlayer_Shader& Prototype);
	virtual ~CPlayer_Shader() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_uint m_iCurrentAnimIndex = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CPlayer_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END