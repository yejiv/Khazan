#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Talk_Danjinjar final : public CUI_Panel
{
private:
    enum class TALKSTATE { TALKING, END };
    enum class UIANIMSTATE { ON, OFF, END};
private:
    CUI_Talk_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Talk_Danjinjar(const CUI_Talk_Danjinjar& Prototype);
    virtual ~CUI_Talk_Danjinjar() = default;

public:
    _bool                           isTalking();
    HRESULT                         On_Panel(_int iTalkIndex);
    void                            Off_Panel();
public:
    void                            Update_UITransform(_vector vPos);

public:
    virtual HRESULT			        Initialize_Prototype() override;
    virtual HRESULT			        Initialize_Clone(void* pArg) override;
    virtual void			        Priority_Update(_float fTimeDelta) override;
    virtual void			        Update(_float fTimeDelta) override;
    virtual void			        Late_Update(_float fTimeDelta) override;
    virtual HRESULT			        Render() override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    vector<class CUI_WorldTex*>     m_BG;
    class CUI_WorldTextBox*         m_pText1 = { nullptr };
    _float							m_fAccTime = {};
    UIANIMSTATE						m_eAnimState = { UIANIMSTATE::END };
    TALKSTATE                       m_eTaking = { TALKSTATE::END };
    _float                          m_fSpeedWeight = {};

    FMOD_CHANNEL*                   m_pChannel = { nullptr };
    
    _wstring                        m_wstrFullText = {};
    _wstring                        m_wstrCulText = {};
    _float                          m_fTalktime = {};

    _int                            m_iNextEvent = {};
    _float                          m_fTextSpeed = {};
private:
    HRESULT					        Ready_Component();
    HRESULT                         Ready_Children();

    void						    UI_Animation(_float fTimeDelta);
    void                            Update_Font(_float fTimeDelta);
public:
    static CUI_Talk_Danjinjar*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END