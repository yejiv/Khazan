#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMotionTrail final : public CComponent
{
public:
    typedef struct tagMotionTrailDesc
    {
        _float2             vLifeTime;
        _float4             vStartColor;
        _float4             vTargetColor;
        float               fRimPower;
        float               fRimIntensity;
        float               fEmissiveIntensity;
        bool                isIndividualColor;
        float               fColorUpdateSpeed;
        float               fInterval;
    }MOTIONTRAIL_DESC;

private:
    CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMotionTrail(const CMotionTrail& Prototype);
    virtual ~CMotionTrail() = default;

public:
    virtual HRESULT         Initialize_Prototype() override;
    virtual HRESULT         Initialize_Clone(void* pArg) override;
    void                    Update(const vector<vector<_float4x4>>& PartsBoneMatrices, _float4x4 WorldMatrix, _float fTimeDelta);
    virtual HRESULT         Render() override;

public:
    class CShader*          Get_Shader() { return m_pShader; }
    const MOTIONTRAIL_DESC& Get_Desc() { return m_Desc; }
    void                    Set_Desc(const MOTIONTRAIL_DESC& Desc) { m_Desc = Desc; }

private:
    MOTIONTRAIL_DESC        m_Desc = {};
    class CShader*          m_pShader = { nullptr };
    
    deque<MOTIONTRAIL_SEGMENT> m_Snapshot;

public:
    static CMotionTrail*    Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent*     Clone(void* pArg) override;
    virtual void            Free() override;
};

NS_END