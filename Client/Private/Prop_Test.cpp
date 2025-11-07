#include "Prop_Test.h"

#include "GameInstance.h"
#include "Body.h"

CProp_Test::CProp_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Test::CProp_Test(const CProp_Test& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Test::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Test::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    PROP_TEST_DESC* pDesc = static_cast<PROP_TEST_DESC*>(pArg);

    _matrix matWorld = XMLoadFloat4x4(&pDesc->WorldMatrix);

    //m_pTransformCom->Set_State(STATE::RIGHT, matWorld.r[0]);
    //m_pTransformCom->Set_State(STATE::UP, matWorld.r[1]);
    //m_pTransformCom->Set_State(STATE::LOOK, matWorld.r[2]);
    //m_pTransformCom->Set_State(STATE::POSITION, matWorld.r[3]);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    return S_OK;
}

void CProp_Test::Priority_Update(_float fTimeDelta)
{
}

void CProp_Test::Update(_float fTimeDelta)
{
    m_pBodyCom->Update(fTimeDelta, m_pTransformCom);
}

void CProp_Test::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CProp_Test::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Test : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(m_eShaderPass)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CProp_Test::Ready_Components(void* pArg)
{
    PROP_TEST_DESC* pDesc = static_cast<PROP_TEST_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL(LEVEL::END, eLevel, E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProp_Test::Ready_Collision(void* pArg)
{
    CBody::BODY_MESHSHAPE_DESC BodyDesc{};
    BodyDesc.pModel = m_pModelCom;
    BodyDesc.pTransform = m_pTransformCom;
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Static;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::MESH;
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
    BodyDesc.vShapeOffset = _float3(0.f, 0.0f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CProp_Test::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Test::Bind_Materials(_uint iMeshIndex)
{
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0);

    return S_OK;
}

CProp_Test* CProp_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Test* pInstance = new CProp_Test(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Test"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Test::Clone(void* pArg)
{
    CProp_Test* pInstance = new CProp_Test(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Test"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Test::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pBodyCom);
}
