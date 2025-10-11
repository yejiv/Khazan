#pragma once
#include "GameObject.h"
#include "MultiDelegate.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC
	{
		_float3			vLocalSize, vLocalPos;
		UISPACETYPE		eSpaceTeype;
		_uint			iUIType;

	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	CMultiDelegate<>				OnClick; // 클릭시 호출
	CMultiDelegate<_float, _float>	OnHover; // 마우스 위치 포함
	CMultiDelegate<_bool>			OnVisibleChanged; // 표시 상태 변화


public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Add_Child(CUIObject* pChild);
	void						Remove_Child(CUIObject* pChild);
	void						Update_Visible(_bool bisVisible);
	void						Update_Transform(class CTransform* pTargetTransform = nullptr);


public:
	_bool						Update_Picking(HWND hWnd);
	virtual void				Broadcast_Click();
	virtual void				Broadcast_Hover(_float fMousePosX, _float fMousePosY);

protected:
	HRESULT						Initialize_Screen_UI(UIOBJECT_DESC* pDesc);
	HRESULT						Initialize_World_UI(UIOBJECT_DESC* pDesc);


protected:
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	_float3						m_vLocalPos = {};
	_float3						m_vWorldPos = {};
	_float3						m_vLocalSize = {};
	_float3						m_vWorldSize = {};

	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

	_bool						m_bisVisible = { true };
	_bool						m_bisHovered = { false };

	CUIObject*					m_pParent = { nullptr };

	vector<CUIObject*>			m_Children = {};

	UISPACETYPE					m_eSpaceType = { UISPACETYPE::END };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;


};

NS_END