#include "Map_Spawn.h"

#include "GameInstance.h"

CMap_Spawn::CMap_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CMap_Spawn::CMap_Spawn(const CMap_Spawn& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CMap_Spawn::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CMap_Spawn::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    SPAWN_DESC* pDesc = static_cast<SPAWN_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_strMonsterKey = pDesc->strMonsterKey;

    m_iSubLevel = pDesc->iSubLevel;

    m_eShaderPass = SHADER_PASS::WIREFRAME;

    m_pTransformCom->Scale(_float3(0.005f, 0.005f, 0.005f));

    return S_OK;
}

void CMap_Spawn::Priority_Update(_float fTimeDelta)
{
}

void CMap_Spawn::Update(_float fTimeDelta)
{
}

void CMap_Spawn::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this), );
}

HRESULT CMap_Spawn::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        m_pShaderCom->Bind_RawValue("g_vColor", &m_vWireColor, sizeof(_float4));

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(m_eShaderPass)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CMap_Spawn::Ready_Components(void* pArg)
{
    SPAWN_DESC* pDesc = static_cast<SPAWN_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LEVEL 안넣음"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

CMap_Spawn* CMap_Spawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Spawn* pInstance = new CMap_Spawn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMap_Spawn"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Spawn::Clone(void* pArg)
{
    CMap_Spawn* pInstance = new CMap_Spawn(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMap_Spawn"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Spawn::Free()
{
    __super::Free();
}
