#pragma once

#include "Editor_Defines.h"
#include "Prop.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Editor)

class CProp_Preview final : public CProp
{
private:
	CProp_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Preview(const CProp_Preview& Prototype);
	virtual ~CProp_Preview() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_PrototypeModel(CModel* pModelCom) { m_pModelCom = pModelCom; }

	void Set_MouseMove(_bool isMouseMove) { m_isMouseMove = isMouseMove; }
	void Set_MousePos(_vector vPos) { m_vMousePos = vPos; }

public:
	void Reset_Preview();

private:
	CModel* m_pModelCom = { nullptr };

	_matrix m_ResetMatrix = {};

	_bool m_isMouseMove = { false };
	_vector m_vMousePos = {};

private:
	HRESULT Ready_Components(void* pArg);
	virtual HRESULT Bind_ShaderResources();

	HRESULT Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Preview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END