#pragma once

#include "Client_Defines.h"
#include "MapObject.h"

NS_BEGIN(Engine)
class CTexture;
class CModel_Instance;
NS_END

NS_BEGIN(Client)

class CProp abstract : public CMapObject
{
public:
	typedef struct tagPropDesc : public CMapObject::MAPOBJECT_DESC
	{
		LEVEL eLevel{ LEVEL::END };

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

protected:
	_tchar m_szModelName[MAX_PATH] = {};
	_bool m_isSnowMap = { true };
	_float m_fSnowAmount = { 0.5f };
	_float3 m_vSnowColor = { 0.92f, 0.94f, 1.f };

protected:
	virtual HRESULT Bind_ShaderResources_ForSnowMap(class CTexture* pTextureCom, _uint iMeshIndex);
	virtual HRESULT Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END