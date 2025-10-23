#pragma once

#include "Editor_Defines.h"
#include "Prop.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Engine)
class CModel_Instance;
NS_END

NS_BEGIN(Editor)

class CProp_Static final : public CProp
{
public:
	typedef struct tagPropStaticDesc : public CProp::PROP_DESC
	{
		_bool isIndependentObject{ false };
		_vector vPosition{};
		_vector vRotation{};
		_vector vScale{};

	}PROP_STATIC_DESC;

	enum class SHADER_PASS { AB, BC, END };			// 수정 예정

private:
	CProp_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Static(const CProp_Static& Prototype);
	virtual ~CProp_Static() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _uint Get_NumInstances() const;

private:
	CModel_Instance* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	virtual HRESULT Bind_ShaderResources();

	HRESULT Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END