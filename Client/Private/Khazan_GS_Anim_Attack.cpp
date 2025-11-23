#include "Khazan_GS_Anim_Attack.h"

CKhazan_GS_Anim_Attack::CKhazan_GS_Anim_Attack()
{
}

HRESULT CKhazan_GS_Anim_Attack::Initialize()
{

    return S_OK;
}

void CKhazan_GS_Anim_Attack::Enter()
{
}

void CKhazan_GS_Anim_Attack::Continue(_float fTimeDelta)
{
}

void CKhazan_GS_Anim_Attack::Exit()
{
}

void CKhazan_GS_Anim_Attack::Reserve_SkillAttack(_uint iSkill)
{
}

void CKhazan_GS_Anim_Attack::Clear_Skill()
{
}

void CKhazan_GS_Anim_Attack::Clear_Attack()
{
}

void CKhazan_GS_Anim_Attack::Clear_All()
{
}

CKhazan_GS_Anim_Attack* CKhazan_GS_Anim_Attack::Create()
{
    CKhazan_GS_Anim_Attack* pInstance = new CKhazan_GS_Anim_Attack;

    if (!pInstance || FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CKhazan_GS_Anim_Attack::Free()
{
    __super::Free();
}
