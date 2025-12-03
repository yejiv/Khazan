#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "PlayerData_Manager.h"

NS_BEGIN(Client)

class CPlayer_Manager final : public CBase
{
public:
    enum CONTROL_BUTTON
    {
        CTRL_LB,
        CTRL_RB,
        CTRL_F,
        Q,
        E,
        R,
        END,
    };

private:
    CPlayer_Manager();
    virtual ~CPlayer_Manager() = default;

public:
    //플레이어 데이터 셋팅 관련
    PLAYER_DATA*                Get_pInitailizePlayerData(); // 플레이어와 연결하면서 초기화까지 같이하기 

    PLAYER_DATA&                Get_ptrPlayerData() {return m_Data; }
    const PLAYER_DATA&          Get_PlayerData() { return m_Data; }
    void                        Initialize_PlayerData(); // Get_pPlayerData()이걸로 데이터 연결 후, 초기화는 이것을 통해 초기화하기

    void						Add_SkillExp(_float fExp);              //스킬 경험치 획득
    _bool						Add_SkillPoint(_int iPoint);            //스킬 포인트 사용/획득

    /* 조작키 관련 */
    /* Spear : iWeapon == 0  */
    /* GSword : iWeapon == 1  */
    void                        BindSkillToButton(CONTROL_BUTTON eButton, _uint iSkill);    //버튼에 스킬을 바인딩
    void                        UnBindSkillToButton(CONTROL_BUTTON eButton);                //버튼에 스킬을 언바인딩 
    _uint                       Get_ButtonSkill(CONTROL_BUTTON eButton);    //해당 키에 스킬이 없으면 반환값 0
    void                        Set_UsedSkill(_uint iSkill, _bool isUsed);  //플레이어에서 갱신해드림! 스킬 사용시
    void                        Set_UsedSkills(_uint iSkill, _bool isUsed);  //플레이어에서 갱신해드림! 여러 스킬 사용시
    void                        Set_UnUsedAllSkills();                      //플레이어 사용되고 있는 모든 스킬 해제
    _bool                       Is_UsedSkill(_uint iSkill);                 //어떤 스킬이 사용됐는지

public:
    HRESULT						Initialize();

private:
    PLAYER_DATA					            m_Data = {};
    /* 조작키 관련 */
    unordered_map<CONTROL_BUTTON, _uint>    m_ButtonToSpearSkill;
    vector<_bool>                           m_UsedSpearSkill;

    unordered_map<CONTROL_BUTTON, _uint>    m_ButtonToGSwordSkill;
    vector<_bool>                           m_UsedGSwordSkill;

    class CClientInstance*                   m_pClientInstance = { nullptr };

public:
	static CPlayer_Manager*		Create();
	virtual void				Free() override;
};

NS_END