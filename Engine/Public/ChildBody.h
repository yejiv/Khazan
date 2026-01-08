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

class ENGINE_DLL CChildBody final : public CBase
{
public:
    typedef struct tagChildBody
    {
        class CModel* pModel;        
        _int iBoneIndex;
        _uint iObjectLayer;
        class CTransform* pOwnerTransform;
        _int iDepth = 0;

        _float fGravity = 1.f;
        _float fAngularDamping = 1.f;
        _float fLinearDamping = 1.f;
        _float fMass = 1.f;

        _float fMinDistance = 0.6f;
        _float fMaxDistance = 1.1f;
        _float fSpringFrequency = 2.f;
        _float fSpringDamping = 0.9f;

        CLOTHTYPE eType;

        Body* pParentBody;
        Body* pRootBody;
        COLLISION_DESC* pCollisionDesc = nullptr;
    }CHILD_BODY_DESC;


private:
    CChildBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CChildBody() = default;

public:
     HRESULT Initialize(CHILD_BODY_DESC* pDesc);

    void Priority_Update(_float fTimeDelta);
    void Update(_float fTimeDelta, _bool isFrozen);
    void Late_Update(_float fAlpha, _bool isFrozen);

public:
    void ApplyToBones(_float fAlpha, _bool isFrozen);

public:
    void Type_Update(_float fTimeDelta);

public:
    void Cape_Update(_float fTimeDelta);
    void Feeler_Update(_float fTimeDelta);

public:
    void Apply_RootInertia(_float fTimeDelta);
    void ClampToCharacter();
    void HardSnapToAnimationRecursive();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    BodyInterface* m_pBodyInterface = { nullptr };
    Body* m_pBody = { nullptr };
    BodyID  m_BodyID;
    Body* m_pParentBody;
    Body* m_pRootBody;
    Ref<DistanceConstraint> m_pConstraint;
    
    vector<class CChildBody*> m_ChildBodys;
    class CTransform* m_pOwnerTransform = { nullptr };
    class CModel* m_pModel = { nullptr };
    class CBone* m_pBone = { nullptr };
    _wstring m_strBoneName;
    _int m_iBoneIndex = {};
    _int m_iDepth = {};

    _float3 m_vRestLocalPos = _float3(0.f, 0.f, 0.f);
    _float4 m_vRestLocalRot = _float4(0.f, 0.f, 0.f, 0.f);

    EMotionType				m_eMotion = EMotionType::Dynamic;
    _uint					m_iObjectLayer = 0;
    _float4x4               m_CombindMatrix = {};

    _float m_fGravity = 1.f;
    _float m_fAngularDamping = 1.f;
    _float m_fLinearDamping = 1.f;
    _float m_fMass = 1.f;
           
    _float m_fMinDistance = 0.6f;
    _float m_fMaxDistance = 1.1f;
    _float m_fSpringFrequency = 2.f;
    _float m_fSpringDamping = 0.9f;
    CLOTHTYPE m_eClothType;

    Vec3 m_vPrevRootVel = Vec3::sZero();
    _bool m_isPrevRootVel = false;

    _float m_fFeelerTime = {};

private:
    HRESULT Ready_Child(CHILD_BODY_DESC* pDesc);
    HRESULT Ready_Body(CHILD_BODY_DESC* pDesc);

    void Limit_Velocity();    

public:
    static CChildBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CHILD_BODY_DESC* pDesc);
    virtual void Free() override;

};

NS_END