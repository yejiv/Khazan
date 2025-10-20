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

		_bool isSnow{ false };
		_bool isCollider{ false };
		_bool isBlended{ false };

		_tchar szModelName[MAX_PATH] = {};

	}PROP_DESC;

	enum class SHADER_PASS { MAIN, WIREFRAME, MAP, MAP_BLEND, SNOWMAP, SNOWMAP_BLEND, END };			// 수정 예정

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

	const _bool Get_isSnow() const { return m_isSnow; }
	const _bool Get_isCollider() const { return m_isCollider; }
	const _bool Get_isBlended() const { return m_isBlended; }

	void Set_isSnow(_bool isSnow) { m_isSnow = isSnow; }
	void Set_isCollider(_bool isCollider) { m_isCollider = isCollider; }
	void Set_isBlended(_bool isBlended) { m_isBlended = isBlended; }

	const _uint Get_ShaderPass() const { return static_cast<_uint>(m_eShaderPass); };
	void Set_ShaderPass(_uint iShaderPass);

protected:
	SHADER_PASS m_eShaderPass = { SHADER_PASS::MAP };

protected:
	_tchar m_szModelName[MAX_PATH] = {};

	_bool m_isSnow = { false };
	_bool m_isCollider = { false };
	_bool m_isBlended = { false };

protected:
	_float m_fSnowAmount = { 0.8f };
	_float3 m_vSnowColor = { 0.92f, 0.94f, 1.f };

protected:
	virtual HRESULT Bind_ShaderResources_ForSnowMap(class CTexture* pTextureCom, _uint iMeshIndex);
	virtual HRESULT Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END