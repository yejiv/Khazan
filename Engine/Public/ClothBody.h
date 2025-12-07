#pragma once
#include "Component.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CClothBody final : public CComponent
{
public:
    typedef struct tagClothBody
    {
        class CModel* pModel;
        vector<_int> RootBoneIndices; 
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
    }CLOTH_BODY_DESC;


private:
    CClothBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CClothBody(const CClothBody& Prototype);
    virtual ~CClothBody() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);

private:
    vector<class CRootBody*> m_RootBodys;
    class CModel* m_pModel = { nullptr };
    class CTransform* m_pOwnerTransform = { nullptr };
    vector<_int> m_RootBoneIndices;
    _uint m_iObjectLayer = {};
private:
    HRESULT Ready_Root(void* pArg);

public:
    static CClothBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

};

NS_END