#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMotionTrail final : public CComponent
{
public:
    typedef struct tagMotionTrailDesc
    {
        class CModel*           pOwnerMasterModel;
        _bool                   HasPartModels;
        vector<class CModel*>   OwnerPartModels;
        MOTIONTRAIL_CONFIG      Config;
    }MOTIONTRAIL_DESC;

    typedef struct tagFrameSnapshot
    {
        vector<XMFLOAT4X4>      BoneCombinedMatrices;   // Combined 행렬들
        XMFLOAT4X4              OwnerWorldMatrix;       // 월드 행렬들
        XMFLOAT2                vLifeTime;
        XMFLOAT3                vStartColor;
        XMFLOAT3                vTargetColor;
    }FRAME_SNAPSHOT;

private:
    CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMotionTrail(const CMotionTrail& Prototype);
    virtual ~CMotionTrail() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize_Clone(void* pArg) override;
    virtual HRESULT             Render() override;
    
public:
    void                        Update(_float fTimeDelta);
    void                        Start_MotionTrail(_float fDuration);
    void                        Update_PartModels(const vector<class CModel*>& NewPartModels);
    void                        Update_MasterModel(class CModel* pModel);

public:
    const MOTIONTRAIL_CONFIG&   Get_Config() { return m_Config; }
    void                        Set_Config(const MOTIONTRAIL_CONFIG& Config) { m_Config = Config; }
    void                        Set_Config(_wstring strConfig);
    void                        Set_Enable(_bool isEnable) { m_isEnable = isEnable; }
    _bool                       isEnable() { return m_isEnable; }

private:
    class CShader*              m_pShader = { nullptr };
    class CModel*               m_pOwnerMasterModel;
    _bool                       m_HasPartModels;
    vector<class CModel*>       m_OwnerPartModels;
    MOTIONTRAIL_CONFIG          m_Config = {};


    deque<FRAME_SNAPSHOT>       m_FrameHistory = {};

    _float                      m_fCaptureTimeAcc = {};
    _float                      m_fColorTimeAcc = {};
    _bool                       m_isEnable = {};

    _bool                       m_isDurationMode = {};
    _float                      m_fDurationTimeAcc = {};
    _float                      m_fDuration = {};

    /* 자주 쓰는 모션트레일 구조체 저장 - 키값으로 관리  */
    unordered_map<_wstring, MOTIONTRAIL_CONFIG>     m_CachedConfig; 

private:
    HRESULT                     Render_PartModel(class CModel* pModel);
    HRESULT                     Ready_CachedConfig();

public:
    static CMotionTrail*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent*         Clone(void* pArg) override;
    virtual void                Free() override;
};

NS_END