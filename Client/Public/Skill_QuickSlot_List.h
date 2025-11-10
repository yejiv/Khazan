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
class CSkill_QuickSlot_List final : public CUI_Tap
{
private:
	CSkill_QuickSlot_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_QuickSlot_List(const CSkill_QuickSlot_List& Prototype);
	virtual ~CSkill_QuickSlot_List() = default;

public:
    void                                UnEquipSlot(_int iSkillIndex);
    void                                Set_SkillIndex(_int iSKillIndex);
    void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY);
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

    _int                                m_iIndex = { -1 };
    _int                                m_iSkillIndex = {};
    _int                                m_iEquipSkillIndex = {};
    vector<class CUI_Atlas_Icon*>       m_pGuideIcon;
    class CUI_Atlas_Icon*               m_pLineIcon = { nullptr };
    class CUI_Atlas_Icon*               m_pSkillIcon = { nullptr };
    class CUI_TextBox*                  m_pSkillName = { nullptr };
private:
	HRESULT								Ready_Componet();
public:
	static CSkill_QuickSlot_List*            Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*                Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END