#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_WorldList final : public CUI_Panel
{
private:
    CUI_WorldList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_WorldList(const CUI_WorldList& Prototype);
    virtual ~CUI_WorldList() = default;

public:
    void                            Update_UITransform(_matrix vParentMat);
    void                            Set_LocalPos(_vector vPos);
    void                            Set_LocalSize(_float3 vSize);

    void                            Setting_Icon(_int iTexPass, _float4 vUV);
    void                            Set_Text(_wstring wstrText);
    void                            Set_Selete(_bool isSelete);

    void                            ON_FX();
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

    _float4x4				        m_CombinedWorldMatrix = {};

    class CUI_WorldTextBox*         m_pName = { nullptr };
    class CUI_WorldTex*             m_pIcon = { nullptr };
    class CUI_WorldTex*             m_pSelete = { nullptr };
    class CUI_WorldTex*             m_pSeleteIcon = { nullptr };
    class CUI_WorldFX*              m_pFX = { nullptr };

    vector<class CUI_WorldTex*>     m_BG;

    _float                          m_fAccTime = {};
    _bool                           m_isUpTime = { false };
    
    _float                          m_fOffsetZ = {};
private:
    HRESULT					        Ready_Component();
    HRESULT                         Ready_Children();
public:
    static CUI_WorldList*           Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END