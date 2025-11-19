#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_WorldFX final : public CUI_Panel
{
private:
    CUI_WorldFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_WorldFX(const CUI_WorldFX& Prototype);
    virtual ~CUI_WorldFX() = default;
public:
    void                    Update_UITransform(_matrix vParentMat);

    void                    Set_LocalPos(_vector vPos);
    void                    Set_LocalSize(_float3 vSize);
    void                    Start_Anim();
public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

private:
    CShader*                m_pShaderCom = { nullptr };
    CTexture*               m_pTextureCom = { nullptr };
    CVIBuffer_Rect*         m_pVIBufferCom = { nullptr };
    _float4x4				m_CombinedWorldMatrix = {};

    _float                  m_fAccTime = {};
private:
    HRESULT					Ready_Component();

public:
    static CUI_WorldFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END