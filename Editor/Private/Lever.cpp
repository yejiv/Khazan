#include "Lever.h"

#include "GameInstance.h"

CLever::CLever(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CLever::CLever(const CLever& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CLever::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CLever::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ANIM_STATE::IDLE1);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CLever::Priority_Update(_float fTimeDelta)
{
}

void CLever::Update(_float fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }
}

void CLever::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CLever::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CLever::Ready_Components(void* pArg)
{
    LEVER_DESC* pDesc = static_cast<LEVER_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

CLever* CLever::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLever* pInstance = new CLever(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLever"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLever::Clone(void* pArg)
{
    CLever* pInstance = new CLever(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLever"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLever::Free()
{
    __super::Free();


}
