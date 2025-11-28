#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CSkipButton final : public CUI_Panel
{
public:
    typedef struct tagSkipButtonTag
    {
        _bool               isOpen;
        function<void()>    Event;
    }SKIPBUTTON_DESC;
private:
    CSkipButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkipButton(const CSkipButton& Prototype);
    virtual ~CSkipButton() = default;

public:
    virtual HRESULT				Initialize_Prototype(_uint iLevel);
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;
    virtual HRESULT				Render() override;
    virtual	HRESULT			    Update_Switch(void* pArg);
private:
    class CUI_TextBox*          m_pTextBox = { nullptr };
    class CSkipButton_Progress* m_pGauge = { nullptr };

    GUIDE_TYPE					m_eGuideType = {};
    _bool						m_bPrePressingState = { false };
    _bool						m_isPressing = { false };
    _float						m_fDelayTime = {};
    _float						m_fAccTime = {};
    _bool						m_bIsFiash = {};

    function<void()>			m_Event;
private:
    HRESULT						Ready_Prototype();
    HRESULT						Ready_Children();

public:
    static CSkipButton*         Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
