#include "BigChest.h"

#include "GameInstance.h"

CBigChest::CBigChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CBigChest::CBigChest(const CBigChest& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CBigChest::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CBigChest::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::CLOSE;
    m_pModelCom->Set_Animation(ANIM_STATE::CLOSE);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CBigChest::Priority_Update(_float fTimeDelta)
{
}

void CBigChest::Update(_float fTimeDelta)
{
    // CLOSE > OPENING > OPEN > CLOSING >> CLOSE                    // 상자 상호 작용 ( 서순 )

    // 7 > 8 > 7 > 8 > 7 . . .

    _bool isisisis = { false };

    if (m_pGameInstance->Key_Down(DIK_7))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::OPENING;
    }
    if (m_pGameInstance->Key_Down(DIK_8))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::CLOSING;
    }

    if (isisisis == true)
    {
        // 열고 닫기
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->AnimationLoop(false);
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (ANIM_STATE::CLOSING == m_eAnimState)
        {
            // 처음 상호 작용 후 애니메이션 루프로 전환
            m_eAnimState = ANIM_STATE::CLOSE;
            m_pModelCom->Set_Animation(ANIM_STATE::CLOSE);
            m_pModelCom->Set_AnimationLoop(true);
        }
        if (ANIM_STATE::OPENING == m_eAnimState)
        {
            // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
            m_eAnimState = ANIM_STATE::OPEN;
            m_pModelCom->Set_Animation(ANIM_STATE::OPEN);
            m_pModelCom->Set_AnimationLoop(true);
        }
    }
}

void CBigChest::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CBigChest::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

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

HRESULT CBigChest::Ready_Components(void* pArg)
{
    BIGCHEST_DESC* pDesc = static_cast<BIGCHEST_DESC*>(pArg);
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

CBigChest* CBigChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBigChest* pInstance = new CBigChest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBigChest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBigChest::Clone(void* pArg)
{
    CBigChest* pInstance = new CBigChest(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBigChest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBigChest::Free()
{
    __super::Free();


}
