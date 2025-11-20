#pragma once
#include "UI_Panel.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CAmount_Info final : public CUI_Panel
{
public:
    typedef struct AMOUNT_Info {
        _float2 iOffsetPos;
        _int iGetValue;
    }AMOUNTINFO_DESC;
private:
    CAmount_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAmount_Info(const CAmount_Info& Prototype);
    virtual ~CAmount_Info() = default;

public:
    virtual HRESULT				Initialize_Prototype();
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;
    virtual HRESULT				Render() override;

    virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
    virtual	HRESULT				Update_Switch(void* pArg);
private:
    CShader*                    m_pShaderCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };
    CVIBuffer_Rect*             m_pVIBufferCom = { nullptr };

    class CUI_Atlas_Icon*       m_pCulIcon = { nullptr };
    class CUI_Atlas_Icon*       m_pGetIcon = { nullptr };

    class CUI_TextBox*          m_pName = { nullptr };

    class CUI_TextBox*          m_pCulValue_Name = { nullptr };
    class CUI_TextBox*          m_pGetValue_Name = { nullptr };
    class CUI_TextBox*          m_pCulValue = { nullptr };
    class CUI_TextBox*          m_pGetValue = { nullptr };

private:
    HRESULT						Ready_Componet();

public:
    static CAmount_Info*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
