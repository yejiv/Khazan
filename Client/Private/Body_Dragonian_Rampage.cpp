#include "Body_Dragonian_Rampage.h"
#include "GameInstance.h"

CBody_Dragonian_Rampage::CBody_Dragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeaponObject{ pDevice,pContext }
{
}

CBody_Dragonian_Rampage::CBody_Dragonian_Rampage(const CBody_Dragonian_Rampage& Prototype)
    :CWeaponObject(Prototype)
{
}

_float4x4* CBody_Dragonian_Rampage::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

_float CBody_Dragonian_Rampage::Get_CulTrack()
{
    return *m_pModelCom->Get_CurTrackPosition();
}

HRESULT CBody_Dragonian_Rampage::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;
    return S_OK;
}

HRESULT CBody_Dragonian_Rampage::Initialize_Clone(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pOwnerTransform = pDesc->pOwnerTransform;
    CHECK_NULLPTR(m_pOwnerTransform, E_FAIL);
    Safe_AddRef(m_pOwnerTransform);

    m_pData = pDesc->pData;
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_CombinedWorldMatrix);
    return S_OK;
}

void CBody_Dragonian_Rampage::Priority_Update(_float fTimeDelta)
{
}

void CBody_Dragonian_Rampage::Update(_float fTimeDelta)
{
    if (m_iPreAnim != m_pData->iAnimIndex)
    {
        m_pModelCom->Set_Animation(m_pData->iAnimIndex);
        m_iPreAnim = m_pData->iAnimIndex;

        if (m_pData->fQuat != 0.f)
        {
            if (!m_pData->isBland)
            {
                m_pModelCom->Set_BlendTime(0.f);
                m_pData->isBland = true;
            }
            else
                m_pModelCom->Set_BlendTime(0.25f);

            _vector vQuat = m_pData->pOwner->Get_Transform()->Get_Rotation_Quat();
            _vector vAddQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_pData->fQuat));
            vQuat = XMQuaternionNormalize(XMQuaternionMultiply(vAddQuat, vQuat));
            m_pData->pOwner->Get_Transform()->Set_Quaternion(vQuat);
            m_pData->fQuat = 0.f;

            if (m_pData->fLook != 0.f)
            {
                _vector		vPosition = m_pData->pOwner->Get_Transform()->Get_State(STATE::POSITION);
                _vector		vLook = m_pData->pOwner->Get_Transform()->Get_State(STATE::LOOK);

                vPosition += XMVector3Normalize(vLook) * m_pData->fLook;
                m_pData->pOwner->Get_Transform()->Set_State(STATE::POSITION, vPosition);
                m_pData->fLook = 0.f;
            }
        }
        else
            m_pModelCom->Set_BlendTime(0.25f);
        
    }

    Update_CombinedMatrix();
    m_pData->isAnimFinash = m_pModelCom->Play_Animation(fTimeDelta);
    Update_Body(fTimeDelta);
}

void CBody_Dragonian_Rampage::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CBody_Dragonian_Rampage::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    CHECK_FAILED(Bind_Dissolve(), E_FAIL);

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fEdgeIntensity = 0.8f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(17);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Dragonian_Rampage::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Dragonian_Rampage::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Dragonian_Rampage::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CBody_Dragonian_Rampage::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Rampage"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pSocketMatrix = m_pModelCom->Get_BoneMatrix("Bip001-Tail4");

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);
    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 1.2f, 0.7f, 0.7f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    BodyDesc.vShapeOffset = _float3(0.f, -0.f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_TailBody"), (CComponent**)&m_pBodyComp, &BodyDesc);

    return S_OK;
}

HRESULT CBody_Dragonian_Rampage::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Dragonian_Rampage::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_pData->fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_pData->fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_pData->fEdgeColor, sizeof(_float4));

    return S_OK;
}

void CBody_Dragonian_Rampage::Update_Body(_float fTimeDelta)
{
    m_pSocketMatrix = m_pModelCom->Get_BoneMatrix("Bip001-Tail4");
    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
    _float4x4 CombinedWorldMatrix = {};

    XMStoreFloat4x4(&CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    _bool isAttakc = m_pData->iAttackBody_State & (_uint)CDragonian_Rampage::ATTACK_BODY::TAIL;
    m_pBodyComp->Collision_Active(isAttakc);
    if (!isAttakc)
        return;

    _matrix WeaponWorld = XMLoadFloat4x4(&CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

    m_pBodyComp->Sync_Update(WeaponWorld);
    m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);

}

CBody_Dragonian_Rampage* CBody_Dragonian_Rampage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CBody_Dragonian_Rampage* pInstance = new CBody_Dragonian_Rampage(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Dragonian_Rampage"));
    }
    return pInstance;
}

CGameObject* CBody_Dragonian_Rampage::Clone(void* pArg)
{
    CBody_Dragonian_Rampage* pInstance = new CBody_Dragonian_Rampage(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Dragonian_Rampage"));
    }

    return pInstance;
}

void CBody_Dragonian_Rampage::Free()
{
    __super::Free();
    Safe_Release(m_pBodyComp);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pTextureCom);
}
