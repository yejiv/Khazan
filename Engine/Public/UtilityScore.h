#pragma once
#include "Base.h"


// ???대뜑????솢? 0~1?ъ씠濡??먯닔瑜?留뚮뱶???꾧뎄
// 釉붾옓蹂대뱶??媛믪쓣 ?쎌뼱??嫄곕━, HP, 荑⑤떎???깆쓣)
// 洹?媛믪쓣 0~1?ъ씠濡??뺢퇋???쒖폒二쇨퀬
// ?щ윭媛쒓? ?덉쓣 寃쎌슦 媛以???怨? 理쒖냼媛믪쑝濡??욎뼱??理쒖쥌 ?먯닔濡?由ы꽩?쒕떎.

namespace UtilityScore 
{
    inline _float Utility_Clamp(_float fX)
    {
        // 0怨??ъ씠濡?議곗젙?댁＜???⑥닔
        if (fX < 0.f) return 0.f;
        if (fX > 1.f) return 1.f;
        return fX;
    }

    inline _float Utility_Remap(_float fValue, _float fMinValue, _float fMaxValue, _bool isInvert = false)
    {
        // Min ~ Max 援ш컙??媛믪쓣 0 ~ 1 濡?留듯븨 ?댁＜???⑥닔
        // Invert媛 True硫??ㅼ쭛?댁꽌 ?묒쓣 ?섎줉 ?먯닔媛 ?щ씪媛???뺥깭濡??ъ슜?쒕떎.

        if (fMaxValue - fMinValue <= 0.00001f)
            return 0.f;

        _float t = (fValue - fMinValue) / (fMaxValue - fMinValue); // 0~1 ?뺢퇋??怨듭떇
        t = Utility_Clamp(t); // ?덉쟾?섍쾶 0~1?ъ씠濡?議곗젙?섎뒗 ?묒뾽

        return isInvert ? (1.f - t) : t; // 留뚯빟 ?ㅼ쭛???듭뀡??二쇰㈃ ?ㅼ쭛?댁꽌 諛섑솚 ?섎룄濡??섎뒗 ?묒뾽
    }

    inline _float DistanceScore(_float fDist, _float MinDist, _float MaxDist)
    {
        // 媛源뚯슱 ?섎줉 ?믪? ?먯닔瑜?諛섑솚?섎룄濡?(fMinDist ??媛源뚯슱?섎줉 ?먯닔媛 ?믪븘吏꾨떎.)
        // ?덈???硫?MinDist媛 洹쇱젒 怨듦꺽 踰붿쐞 ?대㈃ 嫄곌린??媛源뚯썙吏硫??먯닔媛 ?믪븘吏怨?
        // MinDist 媛 以묎굅由?怨듦꺽??媛源뚯썙吏硫??먯닔媛 ?щ씪媛???좏깮?섎뒗 援ъ“媛꾨맂??
        return Utility_Remap(fDist,MinDist,MaxDist,true);
    }

    inline _float LowHPScore(_float fHPPercent, _float fThreshold)
    {
        if (fHPPercent > 1.f) fHPPercent = 1.f;
        if (fHPPercent < 0.f) fHPPercent = 0.f;
        
        // ?⑥? 泥대젰?됱씠 ?꾧퀎媛믩낫???щ㈃ 0??
        if (fHPPercent >= fThreshold)
            return 0.f;

        // ?⑥? 泥대젰?됱씠 ?꾧퀎媛?蹂대떎 ?묒븘吏硫??묒븘吏덉닔濡??먯닔瑜??믨쾶 二쇨쾶?쒕떎.
        return Utility_Clamp((fThreshold - fHPPercent) / fThreshold);

        // 남은 체력량이 임계값 보다 작아지면 작아질수록 점수를 높게 주게된다.

        _float t = (fThreshold - fHPPercent) / fThreshold;

        return Utility_Clamp(t * t);


    }



    // fA : 泥ル쾲吏??먯닔 (0~1)
    // fWA : 泥ル쾲吏??먯닔??媛以묒튂
    // fB : ?먮쾲吏??먯닔 (0~1)
    // fWB : ?먮쾲吏??먯닔??媛以묒튂
    inline _float WeightSum(_float fA, _float fWA, _float fB, _float fWB)
    {
        // Utility???됰룞??理쒖쥌 ?ㅼ퐫?대뒗 蹂댄넻 ?쒓?吏 議곌굔?쇰줈留?寃곗젙?섏? ?딅뒗??
        // 媛?議곌굔 留덈떎 ?낅┰?곸씤 ?먯닔媛 ?덇퀬, ?댁젏?섎뱾??媛以묒튂瑜?遺?ы빐???섎굹濡??⑹튂????솢???섎뒗 ?⑥닔
        return Utility_Clamp(fA * fWA + fB * fWB);
    }

    inline _float AndMul(_float fA, _float fB)
    {
        // A??醫뗪퀬 B??醫뗭븘??理쒖쥌 ?먯닔媛 ?믪븘吏??援ъ“ ?섎굹?쇰룄 ??븘吏硫??먯닔??留ㅼ슦 ??븘吏寃??쒕떎.
        return Utility_Clamp(fA * fB);
    }


    inline _float MaintainScore(_bool isExecuting)
    {
        // 스킬이 진행 중이면 UtilitySelector가 즉시 갈아끼지 않도록 유지 점수를 주도록하는 기능.
        // 이 값을 Score 계산시 WeightSum 해서 넣는다.
        return isExecuting ? 1.0f : 0.f;
    }


    inline _float CooldownScore(_float fRemain, _float fMax)
    {
        // 쿨타임이 끝났으면 1 쿨타임이 남아있으면 0 근처
        return UtilityScore::Utility_Remap(fRemain, 0.f, fMax, true);
    }
    
}
