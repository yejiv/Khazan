#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Khazan_GS_StateMachine.h"

NS_BEGIN(Client)


using   GS_COMMAND = CKhazan_GS_StateMachine::GS_COMMAND;
using   GS_MAIN = CKhazan_GS_StateMachine::GSSM_MAIN;
using   GS_MOVE = CKhazan_GS_StateMachine::GSSM_MOVE;
using   GS_ATTACK = CKhazan_GS_StateMachine::GSSM_ATTACK;
using   GS_GUARD = CKhazan_GS_StateMachine::GSSM_GUARD;


class CKhazan_GS_InputHandler final : public CBase
{
//public:
//    typedef struct tagGSwordKhazanCommandDesc
//    {
//        _uint               iMainType = 0;
//        _uint               iSubType = 0;
//        _uint               iDirection = 0;          // DIRECTION_INFO
//        _float              fChargeTime = 0.f;
//        _uint               iSkillID = 0;
//        _bool               isIsPressed = false;     // 눌림/뗌
//        _float              fTimestamp = 0.f;        // 입력 시간
//
//        tagGSwordKhazanCommandDesc() = default;
//        tagGSwordKhazanCommandDesc(_uint mainType, _uint SubType = 0 ) : iMainType(mainType), iSubType(SubType) {}
//    }GS_COMMAND;

private:
    CKhazan_GS_InputHandler();
    virtual ~CKhazan_GS_InputHandler() = default;

public:
    HRESULT                             Initialize();
    void                                Update(_float fTimeDelta);

    //  Command 가져오기 
    const vector<GS_COMMAND>&           Get_CommandsThisFrame() const { return m_CommandsThisFrame; }
   // _bool                               Has_Command(COMMAND_TYPE eType) const;
  GS_COMMAND                          Get_BufferedCommand();

    //  Input Buffer 
    void                                Enable_InputBuffer(_bool bEnable) { m_isInputBufferEnabled = bEnable; }
    _bool                               Is_BufferEmpty() const { return m_InputBuffer.empty(); }


private:
    class CGameInstance*                m_pGameInstance = { nullptr };
    class CClientInstance*              m_pClientInstance = nullptr;

    // 이번 프레임 Command 
    vector<GS_COMMAND>                  m_CommandsThisFrame;

    //  Input Buffer (선입력) 
    _bool                               m_isInputBufferEnabled = true;
    queue<GS_COMMAND>                   m_InputBuffer;
    const _float                        m_fInputBufferTime = 0.3f;
    _float                              m_fBufferTimer = 0.f;

    //  차징 관련 
    _bool                               m_bIsCharging = false;
    _float                              m_fChargeTime = 0.f;
    const _float                        m_fChargeStartDelay = 0.2f;     // 차징 시작 딜레이

    //  이전 프레임 입력 
    _bool                               m_bPrevMouseLB = false;
    _bool                               m_bPrevMouseRB = false;
    _bool                               m_bPrevSpace = false;

private:
    void                                Process_Movement(_float fTimeDelta);
    void                                Process_Combat(_float fTimeDelta);
    void                                Process_Actions(_float fTimeDelta);
    void                                Process_Skills(_float fTimeDelta);

    void                                Add_Command(GS_COMMAND& command);
    void                                Update_InputBuffer(_float fTimeDelta);
    _uint                               Calculate_InputDirection(_float fTimeDelta);



public:
    static CKhazan_GS_InputHandler* Create();
    virtual void Free() override;
};

NS_END