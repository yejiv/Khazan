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

class ENGINE_DLL CSoftBody final : public CComponent
{
public:
    typedef struct tagSoftBody
    {
        _float3		vPos;
        _float4		vQuat;
        _uint		iObjectLayer;

        _float4x4*     FixBoneMatrix;
        class CModel*  pModel;
        _int           iMeshIndex;

        //const CLOTH_MESH_DATA pClothData;
        //const SKELETON_DATA pSkeletonData;
        
        //class CGameObject* pGameObject = nullptr;
        COLLISION_DESC* pCollisionDesc = nullptr;
    }SOFTBODY_DESC;

private:
    CSoftBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSoftBody(const CSoftBody& Prototype);
    virtual ~CSoftBody() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg) override;    

    void Update_SoftBody_Pelvis(_float fTimeDelta, _matrix ParentMatrix);

public:
    void	Collision_Active(_bool isActive);

public:
    virtual void	Set_PosRot(_vector vPos, _vector vRot);
    void           Set_Pos(_vector vPos);
    BodyID           Get_BodyID() const { return m_BodyID; }
    EMotionType      Get_Motion() const { return m_eMotion; }

    _vector         Get_Pos();
    _vector         Get_Rot();


private:
    Body* m_pBody = { nullptr };
    class CModel* m_pModel = { nullptr };
    BodyID			m_BodyID;
    vector<BodyID> m_MeshBodyIDs;
    BodyInterface* m_pBodyInterface = { nullptr };

    EMotionType				m_eMotion = EMotionType::Static;
    _uint					m_iObjectLayer = 0;

    _float4x4*       m_FixBoneMatrix;
    _matrix        m_WolrdBoneMatrix;

private:
    const JPH::Array<Vec3> ConvertToArrayVec3(class CModel* pModel);
    const JPH::Array<Vec3> ConvertToArrayVec3(class CModel* pModel, _uint iMeshIndex, const Vec3& vScale);
    JPH::Array<Vec3> ConvertToHullPoints(class CModel* pModel, _uint iMeshIndex, const Vec3& vScale, Vec3& outCenter, _float& outNormalizeScale);
    const JPH::Array<Float3> ConvertToArrayFloat3(class  CModel* pModel, _uint iIndex);
    const JPH::Array<IndexedTriangle> ConvertToArrayTri(class CModel* pModel, _uint iIndex);    

public:
    static CSoftBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

};

NS_END