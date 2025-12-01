#pragma once
#include "Base.h"

namespace UtilityScore 
{
    inline _float Utility_Clamp(_float fX)
    {
        if (fX < 0.f) return 0.f;
        if (fX > 1.f) return 1.f;
        return fX;
    }

    inline _float Utility_Remap(_float fValue, _float fMinValue, _float fMaxValue, _bool isInvert = false)
    {

        if (fMaxValue - fMinValue <= 0.00001f)
            return 0.f;

        _float t = (fValue - fMinValue) / (fMaxValue - fMinValue); 
        t = Utility_Clamp(t); 

        return isInvert ? (1.f - t) : t; 
    }

    inline _float DistanceScore(_float fDist, _float MinDist, _float MaxDist)
    {
        return Utility_Remap(fDist,MinDist,MaxDist,true);
    }

    inline _float LowHPScore(_float fHPPercent, _float fThreshold)
    {
        if (fHPPercent > 1.f) fHPPercent = 1.f;
        if (fHPPercent < 0.f) fHPPercent = 0.f;
        
        if (fHPPercent >= fThreshold)
            return 0.f;

        return Utility_Clamp((fThreshold - fHPPercent) / fThreshold);

        // 남은 체력량이 임계값 보다 작아지면 작아질수록 점수를 높게 주게된다.

        _float t = (fThreshold - fHPPercent) / fThreshold;

        return Utility_Clamp(t * t);


    }

    inline _float WeightSum(_float fA, _float fWA, _float fB, _float fWB)
    {
      
        return Utility_Clamp(fA * fWA + fB * fWB);
    }

    inline _float AndMul(_float fA, _float fB)
    {
        return Utility_Clamp(fA * fB);
    }


    inline _float MaintainScore(_bool isExecuting)
    {

        return isExecuting ? 1.0f : 0.f;
    }


    inline _float CooldownScore(_float fRemain, _float fMax)
    {
        return UtilityScore::Utility_Remap(fRemain, 0.f, fMax, true);
    }
    
}
