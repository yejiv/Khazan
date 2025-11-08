#pragma once
#include "Base.h"

NS_BEGIN(Engine)

enum class SENSETYPE { SIGHT, DAMAGE, TEAM, END };

typedef struct tagStimulus
{
    SENSETYPE       eType;              // 감각 종류
    _float3         vLocation = {};     // 자극이 발생한 월드 위치
    _float          fTimeStamp = 0.0;   // 자극이 생성된 시간
    _float          fStrength = 0.f;    // 자극의 강도(0..1 또는 damage 값 등)
    _bool           bSensed = { false };  // Perception이 성공적으로 감지했는지 여부
    _float          fVaildTime = 0.f;   // 이 자극을 유효하다고 볼 TTL (초)
    _uint           iDamageType = {};
} STIMULUS;

typedef struct tagPerceived_Desc
{
    unordered_map<SENSETYPE, STIMULUS>  LastStimulus; // 각 Sene별로 마지막 자극을 기록
    _float                              fLastUpdated = 0.f; // 마지막으로 업데이트 된 시간
    _float                              fLastSeenTime = 0.f; // 시각 감지의 마지막 시간
    _bool                               isCurrentlySensed = false; // 현재 유효 감지 중인지
    _float                              fVisibilityScore = 0.f; // 가중치를 저장 우선순위 결정에 유용(1-> 분명, 0 전혀)
    _float                              fForgetTime = 0.f; // 마지막 감지 후 얼마 뒤에 잊을 지  

}PERCEIVED_DESC;


typedef struct tagSightDesc
{
    _float  fRadius = 0.f; // 감지 반경
    _float  fFov = 90.f; //시야각
    _float  fLoseSightTime = 0.f; // 마지막으로 본후 시야를 잃기 전까지 유지되는 시간
    _float  fCheckInterval = 0.2f; // 현제 센스의 업데이트 주기
    _float  fFovCos = 0.f; // 시야각 FOV / 2의 코사인 갑을 미리 저장한다.
    _bool   isRequireLineOfSight = { false }; // LayCast 사용여부
    _float  fHeightOffset = {};    // 객체의 눈의 위치 

}SIGHT_DESC;


typedef function<void(class CGameObject*, const STIMULUS&)> PERCEPTIONCALLBACK;

NS_END