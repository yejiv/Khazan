#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_PlayerHP_BG final : public CUI_Panel
{
private:
	CUI_PlayerHP_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerHP_BG(const CUI_PlayerHP_BG& Prototype);
	virtual ~CUI_PlayerHP_BG() = default;

public:
	virtual HRESULT			Initialize_Prototype(_uint iLevel);
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Bubble_EventCall();
	virtual HRESULT			Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

    const _float*           m_pMaxValue = { nullptr };
    _float                  m_fDefaultValue = {};

    class CUI_Default_Tex*  m_pBG_L = { nullptr };
    class CUI_Default_Tex*  m_pBG_R = { nullptr };

private:
	HRESULT					Ready_Component();

public:
	static CUI_PlayerHP_BG*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END