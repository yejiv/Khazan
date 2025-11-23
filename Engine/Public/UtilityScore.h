#pragma once
#include "Base.h"


// 이 해더의 역활은 0~1사이로 점수를 만드는 도구
// 블랙보드의 값을 읽어서(거리, HP, 쿨다운 등을)
// 그 값을 0~1사이로 정규화 시켜주고
// 여러개가 있을 경우 가중 합/곱/ 최소값으로 섞어서 최종 점수로 리턴한다.

namespace UtilityScore 
{
    inline _float Utility_Clamp(_float fX)
    {
        // 0과1사이로 조정해주는 함수
        if (fX < 0.f) return 0.f;
        if (fX > 1.f) return 1.f;
        return fX;
    }

    inline _float Utility_Remap(_float fValue, _float fMinValue, _float fMaxValue, _bool isInvert = false)
    {
        // Min ~ Max 구간의 값을 0 ~ 1 로 맵핑 해주는 함수
        // Invert가 True면 뒤집어서 작을 수록 점수가 올라가는 형태로 사용한다.

        if (fMaxValue - fMinValue <= 0.00001f)
            return 0.f;

        _float t = (fValue - fMinValue) / (fMaxValue - fMinValue); // 0~1 정규화 공식
        t = Utility_Clamp(t); // 안전하게 0~1사이로 조정하는 작업

        return isInvert ? (1.f - t) : t; // 만약 뒤집는 옵션을 주면 뒤집어서 반환 하도록 하는 작업
    }

    inline _float DistanceScore(_float fDist, _float MinDist, _float MaxDist)
    {
        // 가까울 수록 높은 점수를 반환하도록 (fMinDist 에 가까울수록 점수가 높아진다.)
        // 예를들 면 MinDist가 근접 공격 범위 이면 거기에 가까워지면 점수가 높아지고
        // MinDist 가 중거리 공격에 가까워지면 점수가 올라가서 선택되는 구조간된다.
        return Utility_Remap(fDist,MinDist,MaxDist,true);
    }

    inline _float LowHPScore(_float fHPPercent, _float fThreshold)
    {
        if (fHPPercent > 1.f) fHPPercent = 1.f;
        if (fHPPercent < 0.f) fHPPercent = 0.f;
        
        // 남은 체력량이 임계값보다 크면 0점
        if (fHPPercent >= fThreshold)
            return 0.f;
        // 남은 체력량이 임계값 보다 작아지면 작아질수록 점수를 높게 주게된다.
        return Utility_Clamp((fThreshold - fHPPercent) / fThreshold);

    }



    // fA : 첫번째 점수 (0~1)
    // fWA : 첫번째 점수의 가중치
    // fB : 두번째 점수 (0~1)
    // fWB : 두번째 점수의 가중치
    inline _float WeightSum(_float fA, _float fWA, _float fB, _float fWB)
    {
        // Utility의 행동의 최종 스코어는 보통 한가지 조건으로만 결정되지 않는다.
        // 각 조건 마다 독립적인 점수가 있고, 이점수들에 가중치를 부여해서 하나로 합치는 역활을 하는 함수
        return Utility_Clamp(fA * fWA + fB * fWB);
    }

    inline _float AndMul(_float fA, _float fB)
    {
        // A도 좋고 B도 좋아야 최종 점수가 높아지는 구조 하나라도 낮아지면 점수는 매우 낮아지게 된다.
        return Utility_Clamp(fA * fB);
    }


    
}
