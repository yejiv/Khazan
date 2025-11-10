#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CSkill_Info final : public CUI_Panel
{
public:
    typedef struct Skill_Info {
        _int iSkillIndex;
        _float2 iOffsetPos;
        _bool isEquip;
        _bool isMaxLevel;
        _bool isGet;
        _bool isOnPreSkill;
    }SKILLINFO_DESC;

private:
    CSkill_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_Info(const CSkill_Info& Prototype);
    virtual ~CSkill_Info() = default;

public:
    virtual HRESULT				Initialize_Prototype(_uint iLevel);
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

    _int						m_iSkillIndex = { -1 };

    //vector<class CUI_Atlas_Icon*> m_pEffectIcon;
    //vector<class CUI_TextBox*>	m_pEffectText;
    //vector<class CUI_TextBox*>	m_pEffectValue;

    //class CUI_Default_Tex* m_pTopBg = { nullptr };
    //class CUI_Default_Tex* m_pBottomBg = { nullptr };

    class CUI_TextBox*               m_pSKillName = { nullptr };
    class CUI_TextBox*               m_pSkillGaugeText = { nullptr };

    class CUI_Atlas_Icon*            m_pPointIcon = { nullptr };
    class CUI_TextBox*               m_pPointText = { nullptr };
    class CUI_Atlas_Icon*            m_pGetIcon = { nullptr };

    class CSkill_Info_Panel*         m_pInfoPanel = { nullptr };
    class CSkill_Condition_Panel*    m_pConditionPanel = { nullptr };
    class CSkill_Info_Tex*           m_pInfo_Tex = {nullptr};

    class CUI_Default_Tex*  m_pEquip_Deco = { nullptr };
    class CUI_TextBox*      m_pEquip_Text = { nullptr };
        
private:
    HRESULT						Ready_Prototype();
    HRESULT						Ready_Componet();

public:
    static CSkill_Info*         Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
