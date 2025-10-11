#pragma once
#include "GameObject.h"
#include "MultiDelegate.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum class UI_Alignment
{
	TOP_LEFT,	 TOP_CENTER,	TOP_RIGHT,
	MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT,
	BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
};


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
	void						Set_Alignment(UI_Alignment eAlign) { m_eAlignment = eAlign; }
	UI_Alignment				Get_Alignment() const { return m_eAlignment; }

public:
	class CTexture*				Set_Texture(CGameObject* pGameObject, CTexture* pTexture);
	void						Set_LocalPos(const _float3& vPos) { m_vLocalPos = vPos; }
	_float3						Get_LocalSize() const { return m_vLocalSize; }

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
	_float2						Compute_AlignedPos(_float2 vWorldPos,_float2 vSize);


public:
	_bool						Update_Picking(HWND hWnd);
	virtual void				Broadcast_Click();
	virtual void				Broadcast_Hover(_float fMousePosX, _float fMousePosY);

protected:
	HRESULT						Initialize_Screen_UI(UIOBJECT_DESC* pDesc);
	HRESULT						Initialize_World_UI(UIOBJECT_DESC* pDesc);

protected:
	// 추후에 UtiliMath 같은 클래스 만들어서 정리 예정
	inline _float				Lerp(_float fStart, _float fEnd, _float fTimeDelta)
	{
		return fStart + (fEnd - fStart) * fTimeDelta;
	}

protected:
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	_float3						m_vLocalPos = {};
	_float3						m_vWorldPos = {};
	_float3						m_vLocalSize = {};
	_float3						m_vWorldSize = {};


	_float2						m_vAnchor;   // 앵커
	UI_Alignment				m_eAlignment = { UI_Alignment::TOP_LEFT };


	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

	_bool						m_isVisible = { true };
	_bool						m_isHovered = { false };

	CUIObject*					m_pParent = { nullptr };

	vector<CUIObject*>			m_Children = {};

	UISPACETYPE					m_eSpaceType = { UISPACETYPE::END };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;


};

NS_END