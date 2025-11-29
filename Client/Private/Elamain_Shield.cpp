#include "Elamain_Shield.h"
#include "GameInstance.h"
#include "AI_Controller.h"

CElamain_Shield::CElamain_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CElamain_Shield::CElamain_Shield(const CElamain_Shield& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT CElamain_Shield::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;
    return S_OK;
}

HRESULT CElamain_Shield::Initialize_Clone(void* pArg)
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

    m_pModelCom->Set_Animation(0.f);
    m_pModelCom->Play_Animation(0);
    return S_OK;
}

void CElamain_Shield::Priority_Update(_float fTimeDelta)
{
}

void CElamain_Shield::Update(_float fTimeDelta)
{
    m_pTransformCom->Rotation(XMConvertToRadians(75.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    _bool isAttakc = m_pData->iAttackBody_State & (_uint)CElamain::ATTACK_BODY::HAND_L;
    m_pBodyComp->Collision_Active(isAttakc);

    //m_pModelCom->Play_Animation(fTimeDelta);

    if (!isAttakc)
        return;

    if (m_pGameInstance->Key_Down(DIK_M))
    {
        Safe_Release(m_pBodyComp);
        this->Remove_Component(TEXT("Com_Body_RH"));
        m_pBodyComp = nullptr;

        CBody::BODY_BOXSHAPE_DESC BodyDesc{};
        BodyDesc.vExtent = { 0.7f, 0.5f, 0.4f };
        BodyDesc.eMotion = EMotionType::Kinematic;
        BodyDesc.eQuality = EMotionQuality::Discrete;
        BodyDesc.eShapeType = SHAPE::BOX;
        BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
        BodyDesc.bIsTrigger = true;

        XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
        BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
        XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

        BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        BodyDesc.pCollisionDesc = &m_tCollisionDesc;

        CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc), );

    }
    _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

    m_pBodyComp->Sync_Update(WeaponWorld);
    m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
}

void CElamain_Shield::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this),);
}

HRESULT CElamain_Shield::Render()
{
    CHECK_FAILED(Bind_ShaderResources(), E_FAIL);

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CElamain_Shield::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pData->pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pData->pOwner);
    }
}

void CElamain_Shield::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CElamain_Shield::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CElamain_Shield::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr), E_FAIL);
    CHECK_FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Elamein_Shield"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr), E_FAIL);

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CElamain_Shield::Ready_Collision()
{
    m_tCollisionDesc.pGameObject = this;

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 0.7f, 0.5f, 0.4f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc), E_FAIL);

    return S_OK;
}

HRESULT CElamain_Shield::Bind_ShaderResources()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

CElamain_Shield* CElamain_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CElamain_Shield* pInstance = new CElamain_Shield(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CElamain_Shield"));
    }
    return pInstance;
}

CGameObject* CElamain_Shield::Clone(void* pArg)
{
    CElamain_Shield* pInstance = new CElamain_Shield(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CElamain_Shield"));
    }

    return pInstance;
}

void CElamain_Shield::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);

}
