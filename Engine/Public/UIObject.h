#pragma once
#include "GameObject.h"
#include "MultiDelegate.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum class UI_ALIGNMENT
	{
		TOP_LEFT, TOP_CENTER, TOP_RIGHT,
		MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT,
		BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
	};


public:
	typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC
	{
		_float2			vLocalSize, vLocalPos;
		_float			fDepth;
		_int			iUIType;
		string			szName;

		function<void()> BubbleEvent = nullptr;
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
	void						Set_Alignment(UI_ALIGNMENT eAlign) { m_eAlignment = eAlign; }
	UI_ALIGNMENT				Get_Alignment() const { return m_eAlignment; }

public:
	const string&				Get_Name() { return m_szName; }
	const _float2&				Get_WolrdPos() { return m_vWorldPos ; }
	const _int&					Get_UIType() { return m_iUIType; }
	const _float&				Get_Depth() { return m_fDepth; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual void				Add_Child(CUIObject* pChild);
	virtual void				Remove_Child(CUIObject* pChild);
	virtual void				Update_Visible(_bool bisVisible);
	virtual void				Update_Transform(CUIObject* pParent, _float2 vPos);
	virtual void				Update_Scaling(_float fSize);
	virtual void				Update_Rotation(_float fAngle);
	_float2						Compute_AlignedPos(_float2 vWorldPos, _float2 vSize);

public:
	virtual _bool				IsPick(HWND hWnd);
	
protected:
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

	string						m_szName;

	_float2						m_vLocalPos = {};
	_float2						m_vWorldPos = {};
	_float						m_fDepth = {};

	_float2						m_vLocalSize = {};
	_float2						m_vWorldSize = {};

	_float2						m_vAnchor;
	UI_ALIGNMENT				m_eAlignment = { UI_ALIGNMENT::TOP_LEFT };

	_int						m_iUIType = { -1 };
	
	_bool						m_isVisible = { true };

	vector<CUIObject*>			m_Children = {};

	std::function<void()>		m_UIBubbleCallBack;
	vector<UIKEYFRAME>			m_Track;
	_int						m_iShaderPass = {};
	vector<_float4>				m_vUVMinMax = {};

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;


};

NS_END