#include "Khazan_GS_InputHandler.h"
#include "ClientInstance.h"
#include "GameInstance.h"


CKhazan_GS_InputHandler::CKhazan_GS_InputHandler()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}
HRESULT CKhazan_GS_InputHandler::Initialize()
{
    return S_OK;
}

void CKhazan_GS_InputHandler::Update(_float fTimeDelta)
{
    m_CommandsThisFrame.clear();

    // 선입력 업데이트 먼저
    Update_InputBuffer(fTimeDelta);

    // 입력 처리
    Process_Movement(fTimeDelta);
    Process_Combat(fTimeDelta);
    Process_Actions(fTimeDelta);
    Process_Skills(fTimeDelta);


}

//_bool CKhazan_GS_InputHandler::Has_Command(COMMAND_TYPE eType) const
//{
//    for (const auto& cmd : m_CommandsThisFrame)
//    {
//        if (cmd.iType == eType)
//            return true;
//    }
//    return false;
//}

GS_COMMAND CKhazan_GS_InputHandler::Get_BufferedCommand()
{
    if (m_InputBuffer.empty())
        return GS_COMMAND();

    GS_COMMAND cmd = m_InputBuffer.front();
    m_InputBuffer.pop();

    return cmd;
}

void CKhazan_GS_InputHandler::Process_Movement(_float fTimeDelta)
{
    _uint iDirection = Calculate_InputDirection(fTimeDelta);

    // walk
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && iDirection > 0)
    {
        GS_COMMAND cmd(GS_MAIN::GS_MOVE , GS_MOVE::GS_WALK);
        cmd.iDirection.iDirFlag = iDirection;
        Add_Command(cmd);
    }

    // Sprint
    else if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta) && iDirection > 0)
    {
        GS_COMMAND cmd(GS_MAIN::GS_MOVE , GS_MOVE::GS_WALK);
        cmd.iDirection.iDirFlag = iDirection;
        Add_Command(cmd);
    }

    // Dodge
    else if (m_pGameInstance->Key_Down(DIK_SPACE) && iDirection > 0)
    {
        GS_COMMAND cmd(GS_MAIN::GS_MOVE , GS_MOVE::GS_WALK);
        cmd.iDirection.iDirFlag = iDirection;
        Add_Command(cmd);
    }

    // Dodge
    else if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        GS_COMMAND cmd(GS_MAIN::GS_MOVE , GS_MOVE::GS_WALK);
        cmd.iDirection.iDirFlag = iDirection;
        Add_Command(cmd);
    }

    // Run
    else if (iDirection > 0)
    {
        GS_COMMAND cmd(GS_MAIN::GS_MOVE , GS_MOVE::GS_WALK);
        cmd.iDirection.iDirFlag = iDirection;
        Add_Command(cmd);
    }

}

void CKhazan_GS_InputHandler::Process_Combat(_float fTimeDelta)
{
}

void CKhazan_GS_InputHandler::Process_Actions(_float fTimeDelta)
{
}

void CKhazan_GS_InputHandler::Process_Skills(_float fTimeDelta)
{
}

void CKhazan_GS_InputHandler::Add_Command(GS_COMMAND& command)
{
    m_CommandsThisFrame.emplace_back(command);

    /* 선 입력 활성화 시 */
    if (m_isInputBufferEnabled)
    {
        m_InputBuffer.push(command);
        m_fBufferTimer = 0.f;
    }
}

void CKhazan_GS_InputHandler::Update_InputBuffer(_float fTimeDelta)
{
    if (!m_isInputBufferEnabled || m_InputBuffer.empty())
        return;

    m_fBufferTimer += fTimeDelta;

    if (m_fBufferTimer >= m_fInputBufferTime)
    {
        while (!m_InputBuffer.empty())
            m_InputBuffer.pop();

        m_fBufferTimer = 0.f;
    }

}

_uint CKhazan_GS_InputHandler::Calculate_InputDirection(_float fTimeDelta)
{
    _uint iDirection = 0;

    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
        iDirection |= ENUM_CLASS(DIRECTION::F);
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
        iDirection |= ENUM_CLASS(DIRECTION::B);
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
        iDirection |= ENUM_CLASS(DIRECTION::L);
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
        iDirection |= ENUM_CLASS(DIRECTION::R);

    return iDirection;
}

CKhazan_GS_InputHandler* CKhazan_GS_InputHandler::Create()
{
    CKhazan_GS_InputHandler* pInstance = new CKhazan_GS_InputHandler();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GS_InputHandler::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);


}
