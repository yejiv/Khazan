#pragma once
#include "Base.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CRootBody final : public CBase
{
public:
    typedef struct tagRootBody
    {
        class CModel* pModel;
        _int iBoneIndex;        
        _uint iObjectLayer;
        class CTransform* pOwnerTransform;

        _float fGravity = 1.f;
        _float fAngularDamping = 1.f;
        _float fLinearDamping = 1.f;
        _float fMass = 1.f;

        _float fMinDistance = 0.6f;
        _float fMaxDistance = 1.1f;
        _float fSpringFrequency = 2.f;
        _float fSpringDamping = 0.9f;

        CLOTHTYPE eType;
        COLLISION_DESC* pCollisionDesc = nullptr;
    }ROOT_BODY_DESC;


private:
    CRootBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRootBody() = default;

public:
    HRESULT Initialize(ROOT_BODY_DESC* pDesc);

    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Late_Update(_float fTimeDelta);

    void SyncFromAnimation(_float fTimeDelta);
    void ApplyToBones(_float fBlend);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    Body* m_pBody = { nullptr };
    BodyID  m_BodyID;

    vector<class CChildBody*> m_ChildBodys;
    class CTransform* m_pOwnerTransform = { nullptr };

    class CModel* m_pModel = { nullptr };
    _wstring m_strBoneName;
    _int m_iBoneIndex = {};
    class CBone* m_pBone = { nullptr };

    BodyInterface* m_pBodyInterface = { nullptr };

    EMotionType				m_eMotion = EMotionType::Kinematic;    
    _uint					m_iObjectLayer = 0;

    _float4x4               m_CombindMatrix = {};

    _bool m_isFirstSync = true;
    RVec3 m_vPrevAnimPos = RVec3::sZero();
    Quat  m_qPrevAnimRot = Quat::sIdentity();
    float m_fFreezeTimer = 0.f;

private:
    HRESULT Ready_Child(ROOT_BODY_DESC* pDesc);
    HRESULT Ready_Body(ROOT_BODY_DESC* pDesc);

    void HardSnapToAnimation(const RVec3& pos, const Quat& rot);

public:
    static CRootBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ROOT_BODY_DESC* pDesc);
    virtual void Free() override;

};

NS_END