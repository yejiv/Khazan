#pragma once

#include "Client_Defines.h"
#include "Prop.h"

// 단일 맵 오브젝트

NS_BEGIN(Engine)
class CModel_Instance;
class CBody;
class CDeferredShader;
NS_END

NS_BEGIN(Client)

class CProp_Static final : public CProp
{
public:
	typedef struct tagPropStaticDesc : public CProp::PROP_DESC
	{
		_float4x4 WorldMatrix{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

	}PROP_STATIC_DESC;

	enum class SHADER_PASS { MAIN, WIREFRAME, SHADOW, MAP, MAP_ICE, SNOWMAP, SNOWMAP_ICE, PLANT, SNOWPLANT, END };

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
    virtual HRESULT Render_Shadow() override;
	virtual HRESULT Deferred_Render(ID3D11DeviceContext* pDeferredContext) override;

private:
	CModel_Instance* m_pModelCom = { nullptr };
	CBody* m_pBodyCom = { nullptr };
	CDeferredShader* m_pDeferredShader = { nullptr };
private:
	SHADER_PASS m_eShaderPass = { SHADER_PASS::MAP };

#pragma region 풀때기
private:
	_float m_fTime = {};
	_float3 m_vWindDir = { _float3(1.f, 0.f, 0.35f) };
	_float m_fWindPower = { 0.45f };
	_float m_fWindSpeed = { 2.4f };
#pragma endregion

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_Collision(void* pArg);
	virtual HRESULT Bind_ShaderResources();
	HRESULT Deferred_Bind_ShaderResources();

	HRESULT Bind_Waving_Plants();
	HRESULT Bind_Materials(_uint iMeshIndex);
	HRESULT Deferred_Bind_Waving_Plants();
	HRESULT Deferred_Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END