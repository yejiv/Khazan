#pragma once

#include "Client_Defines.h"
#include "MapObject.h"

NS_BEGIN(Engine)
class CTexture;
class CModel_Instance;
class CDeferredShader;
NS_END

NS_BEGIN(Client)

class CProp abstract : public CMapObject
{
public:
	typedef struct tagPropDesc : public CMapObject::MAPOBJECT_DESC
	{
		LEVEL eLevel{ LEVEL::END };

		MAPOBJECT_PROPERTIES Properties{};

		_float4x4 WorldMatrix{};

		_tchar szModelName[MAX_PATH] = {};

	}PROP_DESC;

protected:
	CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp(const CProp& Prototype);
	virtual ~CProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	_bool isSnow() { return m_Properties.isSnow; }
	_bool isCollider() { return m_Properties.isCollider; }
	_bool isIce() { return m_Properties.isIce; }
	_bool isInstance() { return m_Properties.isInstance; }
	_bool isShadow() { return m_Properties.isShadow; }
	_bool isBackGround() { return m_Properties.isBackGround; }
	_bool isPlant() { return m_Properties.isPlant; }

protected:
	_tchar m_szModelName[MAX_PATH] = {};

	MAPOBJECT_PROPERTIES m_Properties = {};

	_float m_fSnowAmount = { 0.5f };
	_float3 m_vSnowColor = { 0.92f, 0.94f, 1.f };

protected:
	virtual HRESULT Bind_ShaderResources_ForSnowMap(_uint iMeshIndex);
	HRESULT Deferred_Bind_ShaderResources_ForSnowMap(_uint iMeshIndex, CDeferredShader* pDeferredShader);
	virtual HRESULT Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex);
	HRESULT Deferred_Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex, CDeferredShader* pDeferredShader);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END