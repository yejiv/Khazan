#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CMapObject abstract : public CGameObject
{
public:
	typedef struct tagBoxItemDesc
	{
		_uint iItem_0 = {};
		_uint iItem_1 = {};
		_uint iItem_2 = {};

	}ITEMBOX_DESC;

	typedef struct tagMapObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iMapObjectID{};

		INTERACTIVE_TYPE eInteractiveType{ INTERACTIVE_TYPE::END };

		ITEMBOX_DESC ItemBox{};

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

public:
	const _uint Get_MapObjectID() const { return m_iMapObjectID; }
	const INTERACTIVE_TYPE Get_InteractiveType() const { return m_eInteractiveType; }
	const ITEMBOX_DESC Get_ItemBox() const { return m_ItemBox; }
	void Set_ItemBox(ITEMBOX_DESC ItemBoxDesc) { m_ItemBox = ItemBoxDesc; }

protected:
	CShader* m_pShaderCom = { nullptr };

protected:
	_uint m_iMapObjectID = {};

	INTERACTIVE_TYPE m_eInteractiveType{ INTERACTIVE_TYPE::END };
	ITEMBOX_DESC m_ItemBox = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END