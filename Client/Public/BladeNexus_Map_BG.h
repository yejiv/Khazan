#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
#include "UI_Announce_Result.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CBladeNexus_Map_BG final : public CUI_Panel
{
private:
    CBladeNexus_Map_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBladeNexus_Map_BG(const CBladeNexus_Map_BG& Prototype);
    virtual ~CBladeNexus_Map_BG() = default;

public:
    virtual HRESULT					Initialize_Prototype();
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual HRESULT					Render() override;


    virtual HRESULT				    Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:

    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
    CTexture*                       m_pDissolveTextureCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };

private:
    HRESULT							Ready_Component();

public:
    static CBladeNexus_Map_BG*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;
};
NS_END
