#include "DestinyStone.h"

#include "GameInstance.h"

#include "DestinyGem.h"

CDestinyStone::CDestinyStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CDestinyStone::CDestinyStone(const CDestinyStone& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CDestinyStone::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDestinyStone::Initialize_Clone(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_pTransformCom->Scale(_float3(0.01f, 0.01f, 0.01f));

    return S_OK;
}

void CDestinyStone::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CDestinyStone::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CDestinyStone::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CDestinyStone::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(12), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CDestinyStone::Ready_Components(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CDestinyStone::Ready_PartObjects(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CDestinyGem::DESTINYGEM_DESC DestinyStoneDesc = {};

    DestinyStoneDesc.eLevel = eLevel;
    DestinyStoneDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gem"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_DestinyStone_Gem"), &DestinyStoneDesc), E_FAIL);

    return S_OK;
}

CDestinyStone* CDestinyStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDestinyStone* pInstance = new CDestinyStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDestinyStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestinyStone::Clone(void* pArg)
{
    CDestinyStone* pInstance = new CDestinyStone(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDestinyStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestinyStone::Free()
{
    __super::Free();
}
