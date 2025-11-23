#pragma once
#include "Client_Defines.h"
#include "Khazan_GS_Animation.h"

NS_BEGIN(Client)


class CKhazan_GS_Anim_Move final : public CKhazan_GS_Animation
{

public:
    typedef struct	tagKhazanGSwordAnimMove
    {
        _uint				iWeapon = {};// 0 :barehand, 2: spear, 4: gsword
        _uint				iState = {};
        _uint				iCycle = {};
        _bool               isLockOn = {};
        DIRECTION_INFO		eDir;
    }GS_MOVEINFO;

    typedef struct	tagKhazanGSwordFootSpearInfo
    {
        vector<_float>	vLeftFootFrames;	//왼발이 땅에 닿는 프레임
        vector<_float>	vRightFootFrames;	//오른발이 땅에 닿는 프레임
        _float			fAnimDuration = {};	//애니메이션 전체 프레임 길이
    }GS_FOOTINFO;

private:
    enum FOOT { FOOT_R, FOOT_L, FOOT_END };

private:
    CKhazan_GS_Anim_Move();
    virtual ~CKhazan_GS_Anim_Move() = default;

public:
    void			Initialize(); // m_pModel()set 후 따로 호출하자!!!! 

public:
    void Enter() override;
    void Continue(_float fTimeDelta) override;
    void Exit() override;

public:

    /* Moving*/
    _bool	Try_ChangeAnimation(GS_MOVEINFO moveInfo);
    //_bool	Try_Walk(GS_MOVEINFO moveInfo);
    //_bool	Try_Run(GS_MOVEINFO moveInfo);
    //_bool	Try_Sprint(GS_MOVEINFO moveInfo);
    //_bool	Try_Dodge(GS_MOVEINFO moveInfo);

    void	Reserve_Animation(GS_MOVEINFO moveInfo);

    /* turn */
    _bool   Try_Turn180Animation(GS_MOVEINFO moveInfo);

    /* Getter*/
    _bool	IsMoving() const { return m_isMoving; }
    //_bool	IsWalk() const { return m_isWalk; }
    //_bool	IsRun() const { return m_isRun; }
    //_bool	IsSprint() const { return m_isSprint; }
    _bool	IsDodge() const { return m_isDodging; }
    _bool   IsTurning180() const { return m_isTurning180; }
    _bool	IsStopMoveAnimantionFinished() const { return m_isStopAnimationFinished; }

    /* Clear */
    void    Clear();

private:
    /* Foot */
    FOOT		    m_curFoot;
    GS_FOOTINFO	    m_FootInfo[2]; // walk, run 
    _uint		    m_iFootPosition[2] = { 0,0 }; // l,r 몇 번째 프레임

    /* Reserve*/
    GS_MOVEINFO	    m_eReserveInfo = {};

    /* Info */
    _uint           m_iPrevSelectedAnimationIndex = {};
    _uint           m_iCurWeapon = {};

    /* 플래그 */
    _bool		    m_isStopAnimationFinished = { false };
    _bool		    m_isMoving = { false };
    _bool		    m_isWalk = { false };
    _bool		    m_isRun = { false };
    _bool		    m_isSprint = { false };
    _bool		    m_isDodging = { false };
    _bool		    m_isReserve = { false };
    _bool           m_isTurning180 = { false };
    

    /* Table */
    static const string s_strSpearSprinStartAnims[];
    static const string s_strGSSprinStartAnims[];
    static const string s_strSpearDodgeAnims[];
    static const string s_strGSDodgeAnims[];
    _uint   m_iStopAnimIndex[4] ;



private:
    void         Update_FootPosition();
    _bool        Check_SprintStart();
    _bool        Check_Dodge();
    void         Check_Reserve();


    _uint       GetLockOnWalkAnimation(DIRECTION_INFO eDir);
    _uint       GetLockOnRunAnimation(DIRECTION_INFO eDir);
    

    inline _uint    Get_AnimIndexByName(const string& bare, const string& spear, const string& gs);

public:
    static CKhazan_GS_Anim_Move* Create();
    virtual void Free() override;
};

NS_END