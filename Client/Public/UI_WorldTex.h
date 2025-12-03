#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CTexture_Atlas;
NS_END

NS_BEGIN(Client)
class CUI_WorldTex final : public CUI_Panel
{
private:
    CUI_WorldTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_WorldTex(const CUI_WorldTex& Prototype);
    virtual ~CUI_WorldTex() = default;
public:
    void                    Update_UITransform(_matrix vParentMat);
    
    void                    Set_LocalPos(_vector vPos);
    void                    Set_LocalSize(_float3 vSize);
    HRESULT                 Setting_Texture(_int iTexPass, _float4 vUV);
    HRESULT                 Setting_Texture(_int iTexPass, const _wstring& strComponentTag);
    HRESULT                 Setting_Texture(_int iTexPass, const _wstring& strComponentTag, _float4 vUV);

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
    CTexture_Atlas*         m_pAtlasTextureCom = { nullptr };
    CVIBuffer_Rect*         m_pVIBufferCom = { nullptr };
    _float4x4				m_CombinedWorldMatrix = {};

    _bool                   m_IsAtlas = { false };
private:
    HRESULT					Ready_Component();

public:
    static CUI_WorldTex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END