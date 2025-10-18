#pragma once

#include "Editor_Defines.h"
#include "Prop.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Editor)

class CProp_Test final : public CProp
{
public:
	typedef struct tagPropTestDesc : public CProp::PROP_DESC
	{
		_float4x4 WorldMatrix{};

	}PROP_TEST_DESC;

	enum class SHADER_PASS { MAIN, WIREFRAME, MAPOBJECT, END };			// 수정 예정

private:
	CProp_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Test(const CProp_Test& Prototype);
	virtual ~CProp_Test() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };

private:
	SHADER_PASS m_eShaderPass = { SHADER_PASS::MAPOBJECT };

private:
	HRESULT Ready_Components(void* pArg);
	virtual HRESULT Bind_ShaderResources();

	HRESULT Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END