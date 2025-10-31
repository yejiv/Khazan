#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)
class CKhazan_Spear_ASMachine final : public CBase
{
public:
    typedef struct tagKhazanSpearAnimationStateMachine
    {
        /* 현재 무기 상태 */
        _uint   iWeapon = 0;

        /* 주 카테고리*/
        _uint   iCategory = 0;

        /* 보조 카테고리*/
        _uint   iAttack = 0 ;
        _uint   iSkill = 0;
        _uint   iMove = 0;
        _uint   iGuard = 0;         // GUARD enum (추가 예정)
        _uint   iGroggy = 0;        // GROGGY enum (추가 예정)
        _uint   iInteract = 0;      // INTERACT enum (추가 예정)
        _uint   iWeaponChange = 0;  // WEAPON_CHANGE enum (추가 예정)
        _uint   iHold = 0;          // HOLD enum (추가 예정)
        _uint   iDamaged = 0;       // DAMAGED enum (추가 예정)

        /* 정밀 */
        _uint   iDirection = 0;     // 방향 
        _uint   iMoveSub = 0;       // 무브 서브
        _uint   iSet = 0;           // 세트 애니메이션 인덱스 
        _uint   iCycle = 0;         // 생명주기
    }SPEAR_ASM;

    typedef struct tagKhazanSpearCategoryPriority
    {
        wstring strName;
        _uint iCategory;
        _uint iSubType;
        _uint iPriority;
    }SPEAR_PRIORTIY;

    enum WEAPON : _uint
    {
        SPEAR = 1 << 0,
        BAREHAND = 1 << 1,
        NONE = SPEAR | BAREHAND,     //어떤 상태의 무기든 다 가능한 애니메이션
    };

    enum CATEGORY : _uint
    {
        /* 기본초기화값 = x + 5*/
        /* 우선 순위 : 1순위 = 0~9 (x ~ y) */ 
        M_DIE         = 1 << 0,

        /* 우선 순위 : 2순위 = 10~19 */
        M_HOLD        = 1 << 1,
        M_GROGGY      = 1 << 2,
        M_DAMAGED     = 1 << 3,

        /* 우선 순위 : 3순위 = 20~29 */
        M_SKILL       = 1 << 4,

        /* 우선 순위 : 4순위 = 30~39 */
        M_GUARD         = 1 << 5,

        /* 우선 순위 : 5순위 = 40~49 */
        M_ATTACK        = 1 << 6,   // 초기화 값 34
        M_MOVE          = 1 << 7,   // 초기화 값 37

        /* 우선 순위 : 6순위 = 50~59 */
        M_LOCKON        = 1 << 8,

        /* 우선 순위 : 7순위 = 60~69 */
        M_INTERACT      = 1 << 9,
        M_WEAPON_CHANGE = 1<< 10,
    };

    enum ATTACK : _uint
    {
        ATK_FALL        = 1 << 0,
        ATK_FAST        = 1 << 1,
        ATK_GRAPPLE     = 1 << 2,
        ATK_SKILL       = 1 << 3,
        ATK_COUNTER     = 1 << 4,
        ATK_DODGEATK       = 1 << 5,
        ATK_REFLECTION  = 1 << 6,
        ATK_SPRINTATK      = 1 << 7,
        ATK_STRONG      = 1 << 8,
        ATK_JAVELIN     = 1 << 9,
        ATK_CHARGE      = 1 << 10,
    };

 enum SKILL : _uint
    {
        MOONLIGHT_SLASH               = 18,   // 달빛 베기        
        MOONLIGHT_STANCE              = 19,   // 달빛 태세 (패시브)  
        MOONLIGHT_STANCE_VITALITY     = 20,   // 달빛 태세: 활력 (패시브)
        FULL_MOON                     = 21,   // 보름달              
        SHADOW_SLASH                  = 22,   // 그림자 베기           
        SPIRAL_THRUST                 = 23,   // 나선 찌르기          
        SPIRAL_THRUST_WHIRLWIND       = 24,   // 나선 찌르기: 소용돌이
        PURSUIT                       = 25,   // 추격 (패시브)                
        PURSUIT_DEVOUR                = 26,   // 추격: 포식 (패시브)         
        ASSAULT                       = 27,   // 강습                
        MOMENT_SLASH                  = 28,   // 찰나 베기           
        AGILITY                       = 29,   // 기민함 (패시브)                
        CRITICAL_STRIKE               = 30,   // 급소 타격        
        SHADOW_CLEAVE                 = 31,   // 그림자 참격          
        SMASH_DOWN                    = 32,   // 무너뜨리기 (패시브)             
        ENDURANCE                     = 33,   // 인내심 (패시브)              
        WILL_EXTRACTION               = 34,   // 투지 추출 (패시브)        
        COMBATIVE_SPIRIT              = 35,   // 호전적인 투지 (패시브)       
        BRUTAL_ATTACK_EXECUTION       = 36,   // 브루탈 어택: 처형
        BRUTAL_ATTACK_HARVEST         = 37,   // 브루탈 어택: 수확 (패시브)  
        SPEAR_THROW                   = 38,   // 투창 (추후 추가)
        SPEAR_THROW_REPOSE            = 39,   // 투창: 안식 (추후 추가)
        COUNTER_ATTACK                = 40,   // 카운터 어택         
        COUNTER_ATTACK_ONSLAUGHT      = 41,   // 카운터 어택: 공세 (패시브)  
    };

    enum MOVE : _uint
    {
        MOVE_IDLE        = 1 << 0,
        MOVE_WALK        = 1 << 1,
        MOVE_RUN         = 1 << 2,
        MOVE_SPRINT      = 1 << 3,
        MOVE_CLIMB       = 1 << 4,         // 공격보다 높은 순위로 30
        MOVE_MIRAGE_STEP = 1 << 5,   // 조건부 순위로 34
        MOVE_GETUP       = 1 << 6,
        MOVE_FALL        = 1 << 7,
        MOVE_DODGE       = 1 << 8,         // 공격보다 높은 순위로 34
    };

    /* 우선 순위 필요없음 */
    enum MOVESUB : _uint
    {
        MOVESUB_STAND            = 1 << 0,
        MOVESUB_STAND1           = 1 << 1,
        MOVESUB_STAND2           = 1 << 2,
        MOVESUB_STAND_TURN       = 1 << 3,
        MOVESUB_STAND_HARD       = 1 << 4,
    };

    /* 우선 순위 필요없음 */
    enum CYCLE : _uint
    {
        CYCLE_START       = 1 << 0,
        CYCLE_LOOP        = 1 << 1,
        CYCLE_END         = 1 << 2,   
        CYCLE_ENDEND      = 1 << 3,   //사다리 끝에서
        CYCLE_ENDSTART    = 1 << 4,   //사다리 끝에서
        CYCLE_BREAK       = 1 << 5,
    };
    
    enum GUARD : _uint
    {

    };
    
    /* 이거 뺴자 */
    enum GROGGY : _uint
    {

    };

    enum INTERACT : _uint
    {

    };

    /* 이것도 뺴면 될 듯 */
    enum WEAPONCHANGE : _uint
    {

    };

    enum HOLD : _uint
    {

    };


    enum DAMAGED : _uint
    {
        DAMAGED_WEAK = 1 << 0,  //안쓸 듯
        DAMAGED_NORMAL = 1 << 1,
        DAMAGED_STRONG = 1 << 2,
    };


private:
    CKhazan_Spear_ASMachine();
    virtual ~CKhazan_Spear_ASMachine() = default;

public:
	HRESULT    Initialize_Prototype();

public:
	const vector<SPEAR_ASM>& Get_ASMs() const { return m_ASMs; }
	const SPEAR_ASM* Get_ASM(_uint iIndex) const{return iIndex < m_ASMs.size() ? &m_ASMs[iIndex] : nullptr;}

    // 애니메이션 이름으로 인덱스 찾기
    _int Get_AnimIndexByName(const wstring& strName) const;

    // 특정 조건에 맞는 애니메이션 찾기
    vector<_uint> Get_AnimIndicesByCategory(_uint iCategory) const;
    vector<_uint> Get_AnimIndicesByAttack(_uint iAttack) const;
    vector<_uint> Get_AnimIndicesByMove(_uint iMove) const;
    vector<_uint> Get_AnimIndicesByDirection(_uint iDir) const;

    // 특정 카테고리 + 서브타입으로 찾기
    _int Get_AnimIndex(_uint iCategory, _uint iSubType) const;
      
    // 카테고리 우선순위
    _uint Get_CategoryPriority(_uint iCategory, _uint iSubType) const;
    _bool Can_Interrupt(_uint iCurCategory, _uint iCurSubType, _uint iNewCategory, _uint iNewSubType) const;  //우선 순위에 따라 중단시키기

private:
    _uint                                   m_iCurrentWaepon = {};
    _uint                                   m_iCurrentCategory = {};
    _uint                                   m_iCurrentAttack = {};
    _uint                                   m_iCurrentSkill = {};
    _uint                                   m_iCurrentMove = {};

    vector<SPEAR_ASM>                       m_ASMs;
    unordered_map<wstring, _uint>           m_AnimNameToIndex;

    static const SPEAR_PRIORTIY             s_CategoryPriorities[];

private:
	HRESULT         Data_Load();
	_wstring        Load_UTF8ToWString(const std::wstring& filePath);
    _uint           Parse_Weapon(const wstring& str);
    _uint           Parse_Category(const wstring& str);
    _uint           Parse_Attack(const wstring& str);
    _uint           Parse_Skill(const wstring& str);
    _uint           Parse_Move(const wstring& str);
    _uint           Parse_MoveSub(const wstring& str);
    _uint           Parse_Direction(const wstring& str);
    //_uint           Parse_Cycle(const wstring& str);
    //_uint           Parse_Set(const wstring& str);
    //_uint           Parse_Groggy(const wstring& str);
    //_uint           Parse_Interact(const wstring& str);
    //_uint           Parse_WeaponChange(const wstring& str);
    //_uint           Parse_Hold(const wstring& str);
    //_uint           Parse_Damaged(const wstring& str);





    //private:
    //    inline void		Add_State(_uint i) { m_iState |= i; }
    //    inline void		Remove_State(_uint i) { m_iState &= ~i; }
    //    inline _bool	    Has_State(_uint i) { return (m_iState & i) != 0; }
    //    inline _bool	    Has_AllStates(_uint i) { return (m_iState & i) == i; }
    //    inline void		Clear_State() { m_iState = 0; }

public:
    static CKhazan_Spear_ASMachine*   Create();
    virtual void				Free() override;

};

NS_END
