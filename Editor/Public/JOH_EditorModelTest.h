#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CJOH_EditorModelTest final : public CGameObject
{
private:
	CJOH_EditorModelTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJOH_EditorModelTest(const CJOH_EditorModelTest& Prototype);
	virtual ~CJOH_EditorModelTest() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*	m_pShaderCom = { nullptr };
	class CEditor_Model*		m_pModelCom = { nullptr };

	_uint		m_iCurrentAnimIndex = { 0 };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CJOH_EditorModelTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END