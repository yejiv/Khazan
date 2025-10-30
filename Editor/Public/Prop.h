#pragma once

#include "Editor_Defines.h"
#include "MapObject.h"

NS_BEGIN(Engine)
class CTexture;
class CModel_Instance;
NS_END

NS_BEGIN(Editor)

class CProp abstract : public CMapObject
{
public:
	typedef struct tagPropDesc : public CMapObject::MAPOBJECT_DESC
	{
		LEVEL eLevel{ LEVEL::END };

		MAPOBJECT_PROPERTIES Properties{};

		_float4x4 WorldMatrix{};

		_tchar szModelName[MAX_PATH]{};

		_int iSubLevel{};

	}PROP_DESC;

	enum class SHADER_PASS { MAIN, WIREFRAME, SHADOW, MAP, MAP_ICE, SNOWMAP, SNOWMAP_ICE, END };

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
	const _tchar* Get_ModelName() const { return m_szModelName; }

	const MAPOBJECT_PROPERTIES Get_Properties() const { return m_Properties; }

	void Set_Properties(MAPOBJECT_PROPERTIES Properties) { m_Properties = Properties; }

	const _uint Get_ShaderPass() const { return static_cast<_uint>(m_eShaderPass); };
	void Set_ShaderPass(_uint iShaderPass);

	void Set_CheckRender(_bool isCheckRender) { m_isCheckRender = isCheckRender; }
	void Set_RenderProperties(MAPOBJECT_PROPERTIES* pRenderProperties) { m_pRenderProperties = pRenderProperties; }
	void Set_RenderSubLevel(_int* pRenderSubLevel) { m_pRenderSubLevel = pRenderSubLevel; }

	_int Get_SubLevel() { return m_iSubLevel; }
	void Set_SubLevel(_int iSubLevel) { m_iSubLevel = iSubLevel; }

protected:
	SHADER_PASS m_eShaderPass = { SHADER_PASS::MAP };
	_bool m_isCheckRender = { false };
	_int m_iSubLevel = {};

protected:
	_tchar m_szModelName[MAX_PATH] = {};

	MAPOBJECT_PROPERTIES m_Properties = {};

	MAPOBJECT_PROPERTIES* m_pRenderProperties = { nullptr };

	_int* m_pRenderSubLevel = {};

protected:
	_float m_fSnowAmount = { 0.8f };
	_float3 m_vSnowColor = { 0.92f, 0.94f, 1.f };

protected:
	virtual HRESULT Bind_ShaderResources_ForSnowMap(_uint iMeshIndex);
	virtual HRESULT Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END