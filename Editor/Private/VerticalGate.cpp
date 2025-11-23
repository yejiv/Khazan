#include "VerticalGate.h"

#include "GameInstance.h"

CVerticalGate::CVerticalGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CVerticalGate::CVerticalGate(const CVerticalGate& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CVerticalGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CVerticalGate::Initialize_Clone(void* pArg)
{
    VERTICAL_GATE_DESC* pDesc = static_cast<VERTICAL_GATE_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CVerticalGate::Priority_Update(_float fTimeDelta)
{

}

void CVerticalGate::Update(_float fTimeDelta)
{
    if (false == m_isActive && m_pGameInstance->Key_Down(DIK_6))
    {
        m_isActive = true;

        switch (m_eAnimState)
        {
        case ANIM_STATE::IDLE1:
            m_eAnimState = ANIM_STATE::ACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        case ANIM_STATE::IDLE2:
            m_eAnimState = ANIM_STATE::DEACTIVATION;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

        switch (m_eAnimState)
        {
        case ANIM_STATE::ACTIVATION:
            m_eAnimState = ANIM_STATE::IDLE2;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_isActive = false;
            break;
        case ANIM_STATE::DEACTIVATION:
            m_eAnimState = ANIM_STATE::IDLE1;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_isActive = false;
            break;
        }
    }
}

void CVerticalGate::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CVerticalGate::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources ÇÔ¼ö E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(0), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CVerticalGate::Ready_Components(void* pArg)
{
    VERTICAL_GATE_DESC* pDesc = static_cast<VERTICAL_GATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

CVerticalGate* CVerticalGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVerticalGate* pInstance = new CVerticalGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVerticalGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CVerticalGate::Clone(void* pArg)
{
    CVerticalGate* pInstance = new CVerticalGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CVerticalGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVerticalGate::Free()
{
    __super::Free();
}
