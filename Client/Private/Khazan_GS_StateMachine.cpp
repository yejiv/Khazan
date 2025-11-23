#include "Khazan_GS_StateMachine.h"

CKhazan_GS_StateMachine::CKhazan_GS_StateMachine()
{
}

HRESULT CKhazan_GS_StateMachine::Initialize()
{
    /* 높을수록 우선 */
    m_StatePriorityTable[GS_DIE]            = 1000;

    m_StatePriorityTable[GS_FALL]           = 900;
    m_StatePriorityTable[GS_CLIMB]          = 900;
    m_StatePriorityTable[GS_HOLD]           = 900;
    m_StatePriorityTable[GS_JUSTGUARD]      = 900;

    m_StatePriorityTable[GS_DAMAGED]        = 800;
    m_StatePriorityTable[GS_DOWN]           = 800;
    m_StatePriorityTable[GS_UP]             = 800;
    m_StatePriorityTable[GS_GROGGY]         = 800;

    m_StatePriorityTable[GS_SKILL]          = 700;
    m_StatePriorityTable[GS_GUARD]          = 600;
    m_StatePriorityTable[GS_ATTACK]         = 500;
    m_StatePriorityTable[GS_MOVE]           = 400;

    m_StatePriorityTable[GS_LOCKON]         = 100;      //? 이건 우선순위 필요 없을 듯?
    m_StatePriorityTable[GS_INTERACTION]    = 100;

    m_StatePriorityTable[GS_IDLE]           = 10;


    return S_OK;
}

void CKhazan_GS_StateMachine::Update(_float fTimeDelta)
{
    m_isStateChanged = false; 
}

_bool CKhazan_GS_StateMachine::Request_StateChange(const vector<GS_COMMAND>& commands)
{
    if (commands.empty())
        return false;

    _uint       iSelectedIndex = { 0 };

    for (size_t i = 1; i < commands.size(); ++i)
    {
        if (Get_StatePriority(commands[iSelectedIndex].iMainType) >= Get_StatePriority(commands[i].iMainType))
            continue;

        iSelectedIndex = static_cast<_uint>(i);
    }

    m_eCurCommand = commands[iSelectedIndex];

    //m_iCurMainState = commands[iSelectedIndex].iMainType;
    //m_iCurSubState = commands[iSelectedIndex].iSubType;
    //m_eCurDir.iDirFlag = commands[iSelectedIndex].iDirection;

    m_isStateChanged = true;

    return true;
}

void CKhazan_GS_StateMachine::Force_StateChange(_uint iMainState, _uint iSubState)
{
    m_eCurCommand = {};

    m_eCurCommand.iMainType = iMainState;
    m_eCurCommand.iSubType = iSubState;

    //m_iCurMainState = iMainState;
    //m_iCurSubState = iSubState;

    m_isStateChanged = true;

}

_uint CKhazan_GS_StateMachine::Get_StatePriority(_uint iState) const
{
    auto it = m_StatePriorityTable.find(iState);
    if (it != m_StatePriorityTable.end())
        return it->second;

    return 0;
}


CKhazan_GS_StateMachine* CKhazan_GS_StateMachine::Create()
{
    CKhazan_GS_StateMachine* pInstance = new CKhazan_GS_StateMachine();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GS_StateMachine"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GS_StateMachine::Free()
{

    __super::Free();
}
