#include "StateMachine.h"
#include "AI_State.h"
#include "GameObject.h"

CStateMachine::CStateMachine()
{
}

HRESULT CStateMachine::Initialize(CAI_State* pCurrentState)
{
    if (nullptr == pCurrentState)
        return E_FAIL;

    m_pCurrentState = pCurrentState;
    Safe_AddRef(m_pCurrentState);
    m_pCurrentState->Enter(this);

    return S_OK;
}

void CStateMachine::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (nullptr == m_pCurrentState)
        return;

    m_pCurrentState->Update(this, pOwner, fTimeDelta);

}

HRESULT CStateMachine::Set_State(CAI_State* pNextState, CGameObject* pOwner)
{
    if (nullptr == pNextState || pNextState == m_pCurrentState)
        return E_FAIL;

    if (m_pCurrentState)
        m_pCurrentState->Exit(this, pOwner);

    m_pCurrentState = pNextState;

    m_pCurrentState->Enter(this, pOwner);

    return S_OK;
}

HRESULT CStateMachine::Change_State(_uint iStateIndex, CGameObject* pOwner)
{
    CAI_State* pNext = Find_State(iStateIndex);
    if (!pNext)
        return E_FAIL;

    return Set_State(pNext, pOwner);
}

void CStateMachine::OnCollision(COLLISION_DESC* pDesc)
{
    m_pCurrentState->OnCollision(pDesc);
}

HRESULT CStateMachine::Add_State(_uint iStateIndex, CAI_State* pState)
{
    if (nullptr == pState)
        return E_FAIL;

    if (nullptr != Find_State(iStateIndex))
        return E_FAIL;

    m_States.emplace(iStateIndex, pState);
    return S_OK;

}

CAI_State* CStateMachine::Find_State(_uint iStateIndex)
{
    auto iter = m_States.find(iStateIndex);
    if (iter == m_States.end())
        return nullptr;

    return iter->second;
}

void CStateMachine::Free()
{
    __super::Free();

    for (auto& pair : m_States)
        Safe_Release(pair.second);

    m_States.clear();

    Safe_Release(m_pCurrentState);


}
