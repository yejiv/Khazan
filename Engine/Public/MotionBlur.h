#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CMotionBlur final : public CBase
{
private:
    CMotionBlur();
    virtual ~CMotionBlur() = default;

public:
    HRESULT						Initialize();

public:
    HRESULT						Bind_MotionBlur_ShaderResources(class CShader* pShader);
    MOTION_BLUR_DESC		    Get_MotionBlurDesc() { return m_Desc; }
    void						Set_MotionBlurDesc(const MOTION_BLUR_DESC& Desc) { m_Desc = Desc; }
    void                        Set_EnableMotionBlur(_bool isEnable) { m_isEnable = isEnable; }
    void                        Update_PrevMatrices();

private:
    class CGameInstance*        m_pGameInstance = { nullptr };

    _bool                       m_isEnable = {};
    MOTION_BLUR_DESC            m_Desc = {};

    _float4x4                   m_PrevViewMatrix{}, m_PrevProjMatrix{};

public:
    static CMotionBlur*         Create();
    virtual void				Free() override;
};

NS_END