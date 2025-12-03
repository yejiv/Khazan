#pragma once

#include "Client_Defines.h"
#include "Prop.h"

// 단일 맵 오브젝트

NS_BEGIN(Engine)
class CModel;
class CBody;
class CDeferredShader;
NS_END

NS_BEGIN(Client)

class CProp_Object final : public CProp
{
public:
	typedef struct tagPropObjectDesc : public CProp::PROP_DESC
	{

	}PROP_OBJECT_DESC;

	enum class SHADER_PASS { MAIN, WIREFRAME, SOLIDFRAME, SHADOW, MAP, MAP_ICE, SNOWMAP, SNOWMAP_ICE, END };

private:
	CProp_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Object(const CProp_Object& Prototype);
	virtual ~CProp_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Deferred_Render(ID3D11DeviceContext* pDeferredContext) override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pModelCom = { nullptr };
	CBody* m_pBodyCom = { nullptr };
	CDeferredShader* m_pDeferredShader = { nullptr };
private:
	SHADER_PASS m_eShaderPass = { SHADER_PASS::MAP };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_Collision(void* pArg);
	virtual HRESULT Bind_ShaderResources();
	HRESULT Bind_DeferredShaderResources();
	HRESULT Bind_Materials(_uint iMeshIndex);
	HRESULT Deferred_Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END