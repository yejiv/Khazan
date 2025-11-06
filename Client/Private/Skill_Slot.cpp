#include "Skill_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CSkill_Slot::CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice , pContext}
{
}

CSkill_Slot::CSkill_Slot(const CSkill_Slot& Prototype)
    :  CUI_Slot(Prototype)
{
}

HRESULT CSkill_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
	return S_OK;
}

HRESULT CSkill_Slot::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	return S_OK;
}

void CSkill_Slot::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Slot::Update(_float fTimeDelta)
{
}

void CSkill_Slot::Late_Update(_float fTimeDelta)
{
}

CSkill_Slot* CSkill_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_Slot* pInstance = new CSkill_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Slot::Clone(void* pArg)
{
    CSkill_Slot* pInstance = new CSkill_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Slot::Free()
{
	__super::Free();
}
