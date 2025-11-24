#include "Elevator_Gear.h"

#include "GameInstance.h"

CElevator_Gear::CElevator_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject { pDevice, pContext }
{
}

CElevator_Gear::CElevator_Gear(const CElevator_Gear& Prototype)
    : CPartObject { Prototype }
{
}

HRESULT CElevator_Gear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CElevator_Gear::Initialize_Clone(void* pArg)
{
    ELEVATOR_GEAR_DESC* pDesc = static_cast<ELEVATOR_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_pActive = pDesc->pActive;

    m_fOffsetRotation = pDesc->fOffsetRotation;
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fOffsetRotation);

    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CElevator_Gear::Priority_Update(_float fTimeDelta)
{
}

void CElevator_Gear::Update(_float fTimeDelta)
{
    if (true == *m_pActive)
    {
        if (ANIM_STATE::LOOP != m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::LOOP;
            m_pModelCom->Set_Animation(ENUM_CLASS(ANIM_STATE::LOOP));
            m_pModelCom->Set_AnimationLoop(true);
        }
    }
    else
    {
        if (ANIM_STATE::DONE != m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::DONE;
            m_pModelCom->Set_Animation(ENUM_CLASS(ANIM_STATE::DONE));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }

    Update_CombinedMatrix();
}

void CElevator_Gear::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CElevator_Gear::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CElevator_Gear : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 0 칼손잡이 | 1 손 잘림 보호대 | 2 뭐 존나 작은 눈 | 3 밑에 작은 날카로운 | 4 밑에 큰 날카로운 | 5 눈
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CElevator_Gear::Ready_Components(void* pArg)
{
    ELEVATOR_GEAR_DESC* pDesc = static_cast<ELEVATOR_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_Elevator_Gear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CElevator_Gear::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CElevator_Gear::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CElevator_Gear* CElevator_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElevator_Gear* pInstance = new CElevator_Gear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CElevator_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElevator_Gear::Clone(void* pArg)
{
    CElevator_Gear* pInstance = new CElevator_Gear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CElevator_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElevator_Gear::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
