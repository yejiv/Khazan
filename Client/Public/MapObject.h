#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)

class CMapObject abstract : public CGameObject
{
public:
	typedef struct tagMapObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iMapObjectID{};

	}MAPOBJECT_DESC;

protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& Prototype);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() { return S_OK; }

protected:
	CShader* m_pShaderCom = { nullptr };

protected:
	_uint m_iMapObjectID = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END