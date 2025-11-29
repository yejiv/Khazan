#include "Khazan_GS_AnimationController.h"
#include "Khazan_GS_Anim_Move.h"

CKhazan_GS_AnimationController::CKhazan_GS_AnimationController()
{
}

HRESULT CKhazan_GS_AnimationController::Initialize(CModel* pModel, _uint* pCurWaepon, _bool* pLockOn)
{
    m_pModel = pModel;
    Safe_AddRef(m_pModel);
    m_pCurWeapon = pCurWaepon;
    m_pLockOn = pLockOn;

    // 각 애니메이션 모듈 생성
    m_pAnimMove = CKhazan_GS_Anim_Move::Create();
    if (!m_pAnimMove) return E_FAIL;
    m_pAnimMove->Set_Model(m_pModel);
    m_pAnimMove->Initialize();



    return S_OK;
}

void CKhazan_GS_AnimationController::Update(_float fTimeDelta)
{
    if (m_eCommand.iMainType == GS_MAIN::GS_MOVE)
        Execute_Move();

    else if (m_eCommand.iMainType == GS_MAIN::GS_ATTACK)
        Execute_Attack();

    else if (m_eCommand.iMainType == GS_MAIN::GS_GUARD)
        Execute_Guard();

    else if (m_eCommand.iMainType == GS_MAIN::GS_DAMAGED)
        Execute_Damaged();

    else if (m_eCommand.iMainType == GS_MAIN::GS_SKILL)
        Execute_Skill();


}

void CKhazan_GS_AnimationController::Execute_Move()
{
    CKhazan_GS_Anim_Move::GS_MOVEINFO info;
    info.iWeapon = *m_pCurWeapon;
    info.iState = m_eCommand.iSubType;
    info.eDir = m_eCommand.iDirection;
    info.isLockOn = *m_pLockOn;
    info.iCycle = 0;


}

void CKhazan_GS_AnimationController::Execute_Attack()
{
}

void CKhazan_GS_AnimationController::Execute_Guard()
{
}

void CKhazan_GS_AnimationController::Execute_Damaged()
{
}

void CKhazan_GS_AnimationController::Execute_Skill()
{
}

_bool CKhazan_GS_AnimationController::Is_AnimationFinished() const
{
    return _bool();
}

_bool CKhazan_GS_AnimationController::Is_InComboWindow() const
{
    return _bool();
}

_float CKhazan_GS_AnimationController::Get_CurrentAnimProgress() const
{
    return _float();
}

//CKhazan_GS_AnimationController::ANIM_INFO& CKhazan_GS_AnimationController::Create_AnimInfo()
//{
//    ANIM_INFO info;
//
//    info.iAnimIndex = 0;
//}

CKhazan_GS_AnimationController* CKhazan_GS_AnimationController::Create(CModel* pModel, _uint* pCurWaepon, _bool* pLockOn)
{
    CKhazan_GS_AnimationController* pInstance = new CKhazan_GS_AnimationController();

    if (FAILED(pInstance->Initialize(pModel, pCurWaepon, pLockOn)))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GS_AnimationController"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GS_AnimationController::Free()
{
    __super::Free();

    Safe_Release(m_pModel);
    Safe_Release(m_pAnimMove);
}
