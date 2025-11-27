#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLUT final : public CBase
{
private:
    CLUT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLUT() = default;

public:
    HRESULT						Initialize();

public:
    HRESULT                     Bind_LUT_ShaderResources(class CShader* pShader);
    void                        Set_EnableLUT(_bool isEnable) { m_isEnable = isEnable; }
    void                        Set_LUTIntensity(_float fIntensity) { m_Desc.fIntensity = fIntensity; }

private:
    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pContext = { nullptr };

    class CTexture*             m_pTexture = { nullptr };

    LUT_DESC                    m_Desc = {};
    _bool                       m_isEnable = {};

private:
    HRESULT						Ready_LUTTexture();

public:
    static CLUT*                Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void				Free() override;
};

NS_END