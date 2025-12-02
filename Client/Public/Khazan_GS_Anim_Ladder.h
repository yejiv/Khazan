#pragma once

#include "Client_Defines.h"
#include "Khazan_GS_Animation.h"

NS_BEGIN(Client)

class CKhazan_GS_Anim_Ladder final : public CKhazan_GS_Animation
{
public:
    typedef struct tagKhazanGSwordAnimLadder
    {
        _bool   isSprint = { false };
        DIRECTION_INFO eDir;
    }GS_LADDERINFO;

private:
    CKhazan_GS_Anim_Ladder();
    virtual ~CKhazan_GS_Anim_Ladder() = default;


public:
    void			Initialize();
    _bool           Is_Climbing() const { return m_isClimbing; }



public:
    void            Enter() override;
    void            Continue(_float fTimeDelta) override;
    void            Exit() override;
    
public:
    _bool	        Try_Start_Up_Ladder();
    _bool           Try_Start_Down_Ladder(_uint iType);
    _bool	        Force_End_Up_Ladder();
    _bool           Force_End_Down_Ladder();
    _bool           Try_PlayLadder(GS_LADDERINFO info);

private:
    _bool           m_isCurFootLeft = { true };
    _bool           m_isClimbing = { false };
    _bool           m_isSprint = { false };
    _bool           m_isStartClimb = { false };

    _bool           m_isStanding = { false };
    _uint           m_iLadderStartEndAnimIndex[5];


public:
    static CKhazan_GS_Anim_Ladder* Create();
    virtual void Free() override;

};

NS_END



//      CA_P_Kazan_ClimbUp_UL_Loop
//      CA_P_Kazan_ClimbUp_UR_Loop
//      
//      CA_P_Kazan_ClimbStand_L_Loop
//      CA_P_Kazan_ClimbStand_R_Loop
//      
//      CA_P_Kazan_ClimbDn_DL_Loop
//      CA_P_Kazan_ClimbDn_DR_Loop
//      
//      CA_P_Kazan_ClimbDn_D_Sprint_Loop
//      CA_P_Kazan_ClimbDn_D_Sprint_Start