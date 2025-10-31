#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_State_Panel final : public CUI_Panel
{
private:
	CUI_State_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_State_Panel(const CUI_State_Panel& Prototype);
	virtual ~CUI_State_Panel() = default;

public:
	HRESULT								Setting_PanelLevel(_int iPanelType, UI_PLAYER_STATE_DATA* Data);

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	_bool								m_isUpgrade = { false };

	class CUI_TextBox*					m_pName_TextBox = { nullptr };
	class CUI_TextBox*					m_pValue_TextBox = { nullptr };
	class CUI_Atlas_Icon*				m_pIcon = { nullptr };
	
	PLAYTER_STATE						m_eState = { PLAYTER_STATE::END };
	UI_PLAYER_STATE_DATA*				m_pData;
private:
	HRESULT								Ready_Component();
	void								Update_Text();
public:
	static CUI_State_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};
NS_END	
