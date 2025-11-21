#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
#include "UI_BladeNexus.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CCollection_Info final : public CUI_Panel
{
public:
    typedef struct tagCollectionInfoTag
    {
        _int iItemIndex = {};
    }COLLECTIONINFO_DESC;
private:
    CCollection_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCollection_Info(const CCollection_Info& Prototype);
    virtual ~CCollection_Info() = default;

public:
    virtual HRESULT						Initialize_Prototype();
    virtual HRESULT						Initialize_Clone(void* pArg) override;
    virtual void						Priority_Update(_float fTimeDelta) override;
    virtual void						Update(_float fTimeDelta) override;
    virtual void						Late_Update(_float fTimeDelta) override;

    virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
    virtual	HRESULT						Update_Switch(void* pArg);
private:
    class CUI_BackGround*               m_pBackGround = { nullptr };

    class CUI_Atlas_Icon*               m_pAtlasIcon = { nullptr };
    class CUI_TextBox*                  m_pItemName = { nullptr };
    class CUI_TextBox*                  m_pItemInfo = { nullptr };

    _bool                               m_isChangeInputType = {};
    _bool                               m_isOnStart = {};
private:
    HRESULT								Ready_Object();

public:
    static CCollection_Info*            Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END