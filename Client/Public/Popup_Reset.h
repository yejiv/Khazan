#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CPopup_Reset final : public CUI_Panel
{
public:
    typedef struct tagPopupResetTag
    {
        std::function<void()>   Event;
    }POPUP_RESET_DESC;
private:
    CPopup_Reset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPopup_Reset(const CPopup_Reset& Prototype);
    virtual ~CPopup_Reset() = default;

public:
    virtual HRESULT						Initialize_Prototype();
    virtual HRESULT						Initialize_Clone(void* pArg) override;
    virtual void						Priority_Update(_float fTimeDelta) override;
    virtual void						Update(_float fTimeDelta) override;
    virtual void						Late_Update(_float fTimeDelta) override;
    virtual HRESULT						Render() override;

    virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
    virtual	HRESULT						Update_Switch(void* pArg);
private:
    CShader*                            m_pShaderCom = { nullptr };
    CTexture*                           m_pTextureCom = { nullptr };
    CVIBuffer_Rect*                     m_pVIBufferCom = { nullptr };

    class CUI_BackGround*               m_pBackGround = { nullptr };
    class CUI_TextBox*                  m_pSkillPoint = { nullptr };
    std::function<void()>               m_Event;

    _float                              m_fValue = {};

private:
    HRESULT								Ready_Componet();
    HRESULT								Ready_Object();
public:
    static CPopup_Reset*                Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END