#include "Dummy.h"
#include "GameInstance.h"
#include "Body.h"
#include "ContainerObject.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CDummy::CDummy(const CDummy& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CDummy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummy::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(
            m_pGameInstance->Rand(0.f, 100.f),
            0.f,
            m_pGameInstance->Rand(0.f, 100.f),
            1.f
        ));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_Animation(0);

    return S_OK;
}

void CDummy::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CDummy::Update(_float fTimeDelta)
{

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;


    m_pBodyCom->Sync_Update(m_pTransformCom);
    m_pBodyCom->Update(fTimeDelta, m_pTransformCom);
    /*m_pCharVirCom->Sync_Update(m_pTransformCom);
    m_pCharVirCom->Update(fTimeDelta, m_pTransformCom);*/
}

void CDummy::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

}

HRESULT CDummy::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CDummy::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    //m_pBodyCom->Add_Impulse(_float3(0.f, 100.f, 0.f));

    if (ENUM_CLASS(COLLISION_LAYER::PLAYER) == iOtherObjectLayer)
    {
        CTransform* pTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));

        _vector vOtherPos = pTransform->Get_State(STATE::POSITION);
        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

        _vector vDir = vPos - vOtherPos;
        XMVector4Normalize(vDir);

        m_pBodyCom->Set_Velocity(_float3(vDir.m128_f32[0] * 5, vDir.m128_f32[1] * 5, vDir.m128_f32[2] * 5));
        m_pBodyCom->Add_Impulse(1);
        m_pBodyCom->Add_Torque(1);
        m_pBodyCom->Add_Force(1);
    }
}

void CDummy::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

HRESULT CDummy::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    /*CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 0.5f, 0.5f, 0.5f };*/
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    BodyDesc.fRadius = 6.f;
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Dynamic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPos;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    //CCharacterVirtual::CV_BOXSHAPE_DESC tCharVirDesc{};
    //_float3 vPos{};
    //_float4 vQuat{};
    //XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    //XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    //tCharVirDesc.eShapeType = SHAPE::BOX;
    //tCharVirDesc.vPos = vPos;
    //tCharVirDesc.vQuat = vQuat;
    //tCharVirDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    //tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    //tCharVirDesc.pGameObject = this;
    ////tCharVirDesc.vExtent = _float3(1.f, 1.f, 1.f);

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
    //    TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
    //    return E_FAIL;


    return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CDummy::Reset()
{

}

CDummy* CDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummy* pInstance = new CDummy(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDummy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
    CDummy* pInstance = new CDummy(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CDummy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDummy::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBodyCom);
    //Safe_Release(m_pCharVirCom);
}
