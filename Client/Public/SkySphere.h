#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CSkySphere final : public CGameObject
{
public:
	typedef struct tagSkySphereDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eLevel = LEVEL::END;

		SKY_DESC SkyDesc{};

	}SKY_SPHERE_DESC;

private:
	enum TEX_TYPE { DISTANCE_GRADATION, LOOKUP, NORMAL, DISTORTION, NEBULA, STAR_MASK, MOON, RING, TEX_TYPE_END };
	enum MESH_TYPE { SKY, CLOUD, MESH_END };

private:
	CSkySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkySphere(const CSkySphere& Prototype);
	virtual ~CSkySphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	SKY_DESC Get_SkyDesc() { return m_SkyDesc; }
	void Set_SkyDesc(SKY_DESC SkyDesc) { m_SkyDesc = SkyDesc; }

private:
	CModel* m_pModelCom[MESH_END] = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEX_TYPE_END] = {nullptr};
	
	SKY_DESC m_SkyDesc = {};

	_float m_fTimeAcc = { 0.f };

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	
	HRESULT Bind_Sky_ShaderResources();
	HRESULT Bind_Cloud_ShaderResources();

private:
	HRESULT Ready_ImGuiWidget();

public:
	static CSkySphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END