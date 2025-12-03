#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"
#include "UI_State.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_State_List final : public CUI_Tap
{
public:
	enum class STATE_BUTTON_TYPE { UP, DOWN, END };

	typedef struct BubbleDesc : public CUIObject::BUBBLEEVENT
	{
		STATE_BUTTON_TYPE eType;
	}STATE_LIST_BUBBLE;
private:
	CUI_State_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_State_List(const CUI_State_List& Prototype);
	virtual ~CUI_State_List() = default;

public:
	void								Setting_List(_int iIndex, _float2 vPos, _float fOffSetY, _uint* pValue, _int* pUpValue, _int* pPoint);
	void								Setting_Button(_int* pUpValue);

	void								Setting_Type(CUI_State::UI_TYPE eType);

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg);
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	_int								m_iIndex = { -1 };
	_uint*								m_pValue = { nullptr };
	_int*								m_pUpValue = { nullptr };
	_int*								m_pPoint = { nullptr };

	class CUI_TextBox*					m_pName_TextBox = { nullptr };
	class CUI_TextBox*					m_pCulLevel_TextBox = { nullptr };
	class CUI_TextBox*					m_pUpLevel_TextBox = { nullptr };
	class CUI_Atlas_Icon*				m_pStateIcon = { nullptr };
	class CUI_Default_Tex*				m_pUpIcon = { nullptr };
	class CUI_Default_Tex*				m_pHover = { nullptr };
	vector<class CUI_State_Button*>		m_pButton;
	vector<class CUI_Default_Tex*>		m_pTexture;

	CUI_State::UI_TYPE					m_eUI_Type = { CUI_State::UI_TYPE::END };

    _bool                               m_isOver = {};

private:
	HRESULT								Ready_Component();

	void								List_RenderUpdate(_float fTimeDelta);
	void								Button_RenderUpdate(_float fTimeDelta);
public:
	static CUI_State_List*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END
