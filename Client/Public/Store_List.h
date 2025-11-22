#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"
#include "UI_BladeNexus.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CStore_List final : public CUI_Tap
{
private:
    CStore_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CStore_List(const CStore_List& Prototype);
    virtual ~CStore_List() = default;

public:
    void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY, CUIObject* pParet);
    void                                Set_Selete(_bool isSelete);
    void                                Setting_List(_int iItemIndex);
public:
    virtual HRESULT						Initialize_Prototype();
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

    class CStore_Slot*                  m_pSlot = { nullptr };
    class CUI_Default_Tex*              m_pHover = { nullptr };
    class CUI_TextBox*                  m_pItemName = { nullptr };
    class CUI_TextBox*                  m_pGold = { nullptr };


    _bool								m_bIsSelete = { false };
    _float								m_fAccTime = { 1.f };
    _int                                m_iItemIndex = {};

    const _uint*                        m_pPlayerGold = { nullptr };
    _int                                m_iAdd_Gold = {};
private:
    HRESULT								Ready_Componet();
    _wstring				            IntToWstring(_int iValue);
    void								Render_ItemInfo();
public:
    static CStore_List*                 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END