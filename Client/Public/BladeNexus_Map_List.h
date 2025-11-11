#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_BladeNexus_Map_List final : public CUI_Tap
{
private:
    CUI_BladeNexus_Map_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_BladeNexus_Map_List(const CUI_BladeNexus_Map_List& Prototype);
    virtual ~CUI_BladeNexus_Map_List() = default;

public:
    void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY);
    void                                Setting_List(_int iType);
    void								Set_Selete(_bool isSelete);
public:
    virtual HRESULT						Initialize_Prototype(_uint iLevel);
    virtual HRESULT						Initialize_Clone(void* pArg) override;
    virtual void						Priority_Update(_float fTimeDelta) override;
    virtual void						Update(_float fTimeDelta) override;
    virtual void						Late_Update(_float fTimeDelta) override;
    virtual HRESULT						Render() override;

    virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
    CShader*                            m_pShaderCom = { nullptr };
    CTexture*                           m_pTextureCom = { nullptr };
    CVIBuffer_Rect*                     m_pVIBufferCom = { nullptr };

    class CUI_TextBox*                  m_pListText = {nullptr};
    class CUI_Default_Tex*              m_pOverTex = { nullptr };

    _bool								m_bIsSelete = { false };
    _int                                m_iIndex = { -1 };

private:
    HRESULT								Ready_Componet();
public:
    static CUI_BladeNexus_Map_List*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END