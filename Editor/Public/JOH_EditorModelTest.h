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
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void	OnUnEnble() { m_isEnble = false; }
	class CEditor_Model* get_Model() { return m_pModelCom; }

private:
	CShader*	m_pShaderCom = { nullptr };
	class CEditor_Model*		m_pModelCom = { nullptr };

	_uint		m_iCurrentAnimIndex = { 0 };
	_bool		m_isEnble = { true };
private:
	HRESULT Ready_Components(const _wstring& strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CJOH_EditorModelTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END