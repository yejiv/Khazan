#pragma once
#include "Perception_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPerception final : public CBase
{
private:
    CPerception();
    virtual ~CPerception() = default;

public:
    void            Set_PerceptionCallBack(function<void(class CGameObject*, const STIMULUS&)> CallBack)
    {
        m_PerceptionCallBack = CallBack;
    }
public:
    _float              Get_CurrentTime() const { return m_fCurrnetTime; }
    _float              Get_DamageAcc() const { return m_fDamageAcc; }

public:
    HRESULT             Initialize(const AIPERCEPTION_DATA& Desc, _uint iTeamID);
    void                Update(class CGameObject* pOwner, _float fTimeDelta);

public:
    // 데미지 이벤트
    void                Notify_Damage(class CGameObject* pAttacker, const STIMULUS& Stim);

private:
    // 시야 감지
    void                Check_Sight(class CGameObject* pOwner);
    void                Forget();
    void                Forget_Damage();
private:
    string              m_strName = {};
    AIPERCEPTION_DATA   m_tSightDesc{}; // 시야 센스의 설정값을 저장
    _uint               m_iTeamID = { 0 }; // 감지 대상이 아군인지 적군인지 판단
    _float              m_fCheckAcc = { 0.f }; // Update시 누적해서 CheckInterval 만큼 쌓였을때 검사 실행을 위한 변수
    _float              m_fCurrnetTime = { 0.f }; // Update시점에서 현재 시간 Stimulus 생성시 타임 스탬프 기록, PerceivedInfo갱신과 오래된 감지 제거에 사용
    _float              m_LastSweepTime = { 0.f }; // 마지막으로 전체 시야 스캔을 수행한시간
    _float              m_fSenseRadiusCache = { 0.f }; // 센스 반경 최적화 용 캐시 (미리 계산해두고 거리 비교시 사용)
    _float              m_fDamageAcc = { 0.f };
    deque<STIMULUS>     m_DamageHistory;

    unordered_map<class CGameObject*, PERCEIVED_DESC>   m_Perceived; // AI의 현재까지 감지한 모든 대상 정보를 저장하는 테이블
    function<void(class CGameObject*, const STIMULUS&)> m_PerceptionCallBack; // 감지 이베트를 AI_Controller에 알리기 위한 콜백
    
    DIRECTION_INFO      m_tDirInfo = {};


    class CGameInstance*    m_pGameInstance = { nullptr };

public:
    static CPerception*     Create(const AIPERCEPTION_DATA& Desc, _uint iTeamID);
    virtual void            Free() override;


};

NS_END