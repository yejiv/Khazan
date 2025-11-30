#include "Elamein_Sword.h"
#include "GameInstance.h"
#include "AI_Controller.h"

CElamein_Sword::CElamein_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CElamein_Sword::CElamein_Sword(const CElamein_Sword& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT CElamein_Sword::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;
    return S_OK;
}

HRESULT CElamein_Sword::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pData = pDesc->pData;
    m_pSocketMatrix = pDesc->pSocketMatrix;

    m_pOwnerTransform = pDesc->pOwnerTransform;
    CHECK_NULLPTR(m_pOwnerTransform, E_FAIL);
    Safe_AddRef(m_pOwnerTransform);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));
    CHECK_FAILED(Ready_Components(), E_FAIL);
    CHECK_FAILED(Ready_Collision(), E_FAIL);


    return S_OK;
}

void CElamein_Sword::Priority_Update(_float fTimeDelta)
{
}

void CElamein_Sword::Update(_float fTimeDelta)
{
    m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    _bool isAttakc = m_pData->iAttackBody_State & (_uint)CElamein::ATTACK_BODY::SWORD;
    m_pBodyComp->Collision_Active(isAttakc);

    if (!isAttakc)
        return;
    
    _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

    m_pBodyComp->Sync_Update(WeaponWorld);
    m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);

}

void CElamein_Sword::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this),);
}

HRESULT CElamein_Sword::Render()
{
    CHECK_FAILED(Bind_ShaderResources(), E_FAIL);

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CElamein_Sword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pData->pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pData->pOwner);
    }
}

void CElamein_Sword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CElamein_Sword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CElamein_Sword::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr), E_FAIL);
    CHECK_FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Elamein_Sword"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr), E_FAIL);

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CElamein_Sword::Ready_Collision()
{
    m_tCollisionDesc.pGameObject = this;

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 0.4f, 1.f, 0.4f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    BodyDesc.vShapeOffset = _float3(0.f, 0.7f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_LH"), (CComponent**)&m_pBodyComp, &BodyDesc), E_FAIL );


    return S_OK;
}

HRESULT CElamein_Sword::Bind_ShaderResources()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

CElamein_Sword* CElamein_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CElamein_Sword* pInstance = new CElamein_Sword(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CElamein_Sword"));
    }
    return pInstance;
}

CGameObject* CElamein_Sword::Clone(void* pArg)
{
    CElamein_Sword* pInstance = new CElamein_Sword(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CElamein_Sword"));
    }

    return pInstance;
}

void CElamein_Sword::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);

}
