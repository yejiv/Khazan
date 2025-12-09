#pragma once

#include "Editor_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CMapObject abstract : public CContainerObject
{
public:
	typedef struct tagBoxItemDesc
	{
		_int iItem_0 = {};
		_int iItem_1 = {};
		_int iItem_2 = {};

	}ITEMBOX_DESC;

	typedef struct tagMapObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iMapObjectID{};

		INTERACTIVE_TYPE eInteractiveType{ INTERACTIVE_TYPE::END };

        _int iBladeNexus_ID{};
        _int iTombStone_ID{};

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
	const _int Get_BladeNexus_ID() const { return m_iBladeNexusID; }
    const _int Get_TombStoneID() const { return m_iTombStoneID; }
	const ITEMBOX_DESC Get_ItemBox() const { return m_ItemBox; }

    void Set_TombStone_ID(_uint iTombStoneID) { m_iTombStoneID = iTombStoneID; }
    void Set_BladeNexus_ID(_uint BladeNexusID) { m_iBladeNexusID = BladeNexusID; }
	void Set_ItemBox(ITEMBOX_DESC ItemBoxDesc) { m_ItemBox = ItemBoxDesc; }

protected:
	CShader* m_pShaderCom = { nullptr };

protected:
	_uint m_iMapObjectID = {};

	INTERACTIVE_TYPE m_eInteractiveType{ INTERACTIVE_TYPE::END };
	_int m_iBladeNexusID = { 0 };
    _int m_iTombStoneID = { 0 };
	ITEMBOX_DESC m_ItemBox = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END