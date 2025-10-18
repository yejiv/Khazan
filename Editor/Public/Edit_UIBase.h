#pragma once
#include "Editor_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CTexture_Atlas;
NS_END

NS_BEGIN(Editor)
class CEdit_UIBase : public CUIObject
{
public:
	enum class UI_RENDER_TYPE { DEFAULT, ATLAS, END };
	enum class UITYPE { PANEL, TAP, BUTTON, SLOT, SCROLLBAR, PROGRESSBAR, END };

private:
	CEdit_UIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_UIBase(const CEdit_UIBase& Prototype);
	virtual ~CEdit_UIBase() = default;

public:
	HRESULT					Create_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CUIObject::UIOBJECT_DESC* UIChildDesc, string szSeleteUIName, CUIObject* pParent = nullptr);

	//Save/Load
	HRESULT					Save_UI(nlohmann::json& pOutData);
	virtual HRESULT			Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID);

	//IMGUI
	void					Root_SeleteButton(string& szSeleteUIName, _int iNum, _int& iSeletRootUI, _int& iPosX, _int& iPosY, _int& iSizeX, _int& iSizeY);
	void					SeleteButton(string& szSeleteUIName, _int iNum, _int& iPosX, _int& iPosY, _int& iSizeX, _int& iSIzeY);
	void					Update_Option(string& szSeleteUIName, const string pFrameName, _int iTexType);
	_bool					Update_ClassName(string& szSeleteUIName);

	//UI
	_bool					ReName(string& szSeleteUIName, string szChangeUIName);
	_bool					Set_ClassName(string& szSeleteUIName, string szChangeUIName);

	_bool					Move_UI(string& szSeleteUIName, _float fSetX, _float fSetY, CUIObject* pParent, _bool isParent);
	_bool					Scaling_UI(string& szSeleteUIName, _float fSizeX, _float fSizeY);
	void					Set_Alpha(_float fAlpha);

	//Texture
	HRESULT					Set_AtlasTextTure(string& szSeleteUIName, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, const string pFrameName, _int iTexType);
	_bool					Set_UVTexSet(string& szSeleteUIName, const string pFrameName);
	HRESULT					Set_AtlasTexSize(string& szSeleteUIName, const string pFrameName, _float fSize);

	//Anime
	_bool					Anim_Empty(string& szSeleteUIName);
	_bool					Create_Anim(string& szSeleteUIName);
	_bool					Add_Anim(string& szSeleteUIName);
	_bool					Set_Anim(string& szSeleteUIName);
	_bool					Set_AnimPos(string& szSeleteUIName);
	_bool					Get_LastTime(string& szSeleteUIName, _float& fOutTime);
	_bool					Play_Animation(string& szSeleteUIName, _float& fAccTime);
	_bool					ReSet_Track(string& szSeleteUIName);

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CTexture_Atlas* m_pTexture_AtlasCom = { nullptr };

	string					m_szClassName = {};
	string					m_szTexTag = {};
	_int					m_iTexType = {};

	_float4					m_vFrameColor = {};
	_float					m_fAlpha = {};

	_int					m_iUiState = {};
	UI_RENDER_TYPE			m_eRenderType = {};
	_int					m_iTexIndex = {};
	_float					m_fUiState = {};
	_int					m_iUpDownState = {};

	vector<string>			m_EventNames = {};
	_char					m_szEvent[MAX_PATH] = {};

	//Event
	_char					m_szTrackEvent[MAX_PATH] = {};
	_int					m_iSeleteTrackIndex = {};
	_uint					m_iCurrentKeyFrameIndex = {};

private:
	HRESULT					Ready_Component();
	void					Update_Track(_float& fAccTime);

	//Convert
	string					UIType_EnumToString();
	_uint					UIType_StringToEnum(string szUIType);

public:
	static CEdit_UIBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void			Free() override;
};

NS_END