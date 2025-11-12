#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRadialBlur final : public CBase
{
private:
    CRadialBlur();
    virtual ~CRadialBlur() = default;

public:
    HRESULT						Initialize();
    void                        Update(_float fTimeDelta);

public:
    HRESULT						Bind_RadialBlur_ShaderResources(class CShader* pShader);
    RADIAL_BLUR_DESC		    Get_RadialBlurDesc() { return m_Desc; }
    void						Set_RadialBlurDesc(const RADIAL_BLUR_DESC& Desc) { m_Desc = Desc; }
    void                        Set_RadialBlurCenter(_fvector vCenter);
    void                        Start_RadialBlur(_float fDuration, const RADIAL_BLUR_DESC& Desc);

private:
    class CGameInstance*        m_pGameInstance = { nullptr };

    RADIAL_BLUR_DESC            m_Desc = {};

    // Animation
    _float                      m_fTimeAcc = {};
    _float                      m_fDuration = {};

public:
    static CRadialBlur*         Create();
    virtual void				Free() override;
};

NS_END