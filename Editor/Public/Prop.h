#pragma once

#include "Editor_Defines.h"
#include "MapObject.h"

NS_BEGIN(Editor)

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

public:
	const _tchar* Get_ModelName() const { return m_szModelName; }

protected:
	_tchar m_szModelName[MAX_PATH] = {};

protected:
	virtual HRESULT Bind_Instance_Materials(class CEditor_Model_Instance* pModelCom, _uint iMeshIndex);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END