#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
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
	const string&				Get_Name() { return m_szName; }
	const _float2&				Get_WolrdPos() { return m_vWorldPos; }
	const _int&					Get_UIType() { return m_iUIType; }
	const _float&				Get_Depth() { return m_fDepth; }
	void						Get_Data(VTXINSTANCE_UI& pOutData);

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual void				Add_Renderer();
	virtual void				Update_Visible(_bool bisVisible);
	virtual void				Update_Transform(CUIObject* pParent, _float2 vPos);
	virtual void				Update_Scaling(_float fSize);
	virtual void				Update_Rotation(_float fAngle);

	virtual	HRESULT				Update_Switch(void* pArg);

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg);
	virtual void				Bubble_EventCall();
	virtual void				Insert_Bubble(std::function<void()> BubbleEvent);

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

	_int						m_iUIType = { -1 };

	_bool						m_isVisible = { true };

	std::function<void()>		m_UIBubbleCallBack;
	vector<UIKEYFRAME>			m_Track;
	_int						m_iShaderPass = {};
	_int						m_iTexPass = {};
	_float						m_fAlpha = {};

	vector<_float4>				m_vUV = {};
	_int						m_iState = {};

	_bool						m_IsUpdate = { false };
public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;


};

NS_END