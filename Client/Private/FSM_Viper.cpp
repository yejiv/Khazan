#include "FSM_Viper.h"
#include "AS_Idle_Viper.h"
#include "AS_Walk_Viper.h"
#include "AS_Run_Viper.h"
#include "AS_Quick2Hit_Viper.h"
#include "AS_Slow2Hit_VIper.h"
#include "AS_LockOn_Viper.h"

CFSM_Viper::CFSM_Viper()
{
}

HRESULT CFSM_Viper::Initialize()
{
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), CAS_Idle_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::WALK), CAS_Walk_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::RUN), CAS_Run_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::QUICK2HIT), CAS_Quick2Hit_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::SLOW2HIT), CAS_Slow2Hit_VIper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::LOCKON), CAS_LockOn_Viper::Create())))
        return E_FAIL;



    m_pCurrentState = m_States[ENUM_CLASS(VIPER_STATE_P1::IDLE)];
    Safe_AddRef(m_pCurrentState);
    if (nullptr == m_pCurrentState)
        return E_FAIL;
    Safe_AddRef(m_pCurrentState);

    return S_OK;
}

void CFSM_Viper::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner,fTimeDelta);
}

CFSM_Viper* CFSM_Viper::Create()
{
    CFSM_Viper* pInstance = new CFSM_Viper();
    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Viper"));
    }
    return pInstance;
}

void CFSM_Viper::Free()
{
    __super::Free();
}
