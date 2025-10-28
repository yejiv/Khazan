#include "BladeNexus.h"

#include "GameInstance.h"

CBladeNexus::CBladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CBladeNexus::CBladeNexus(const CBladeNexus& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CBladeNexus::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CBladeNexus::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    iiii = BEFORE_IDLE;
    m_pModelCom->Set_Animation(iiii);
    m_pModelCom->Set_AnimationLoop(true);

    return S_OK;
}

void CBladeNexus::Priority_Update(_float fTimeDelta)
{
}

void CBladeNexus::Update(_float fTimeDelta)
{
    // BEFORE_IDLE > _START > _LOOP > _END >>                       // 첫 터치떄
    // >> AFTER_IDLE > _START > _LOOP > _END > AFTER_IDLE >>        // 반복

    // 7 > 9 > 8 > 9 > 8 > 9 >>> ...

    _bool isisisis = { false };

    if (m_pGameInstance->Key_Down(DIK_7))
    {
        isisisis = true;

        iiii = ANIM_STATE::BEFORE_START;
    }
    if (m_pGameInstance->Key_Down(DIK_8))
    {
        isisisis = true;

        iiii = ANIM_STATE::AFTER_START;
    }
    if (m_pGameInstance->Key_Down(DIK_9))
    {
        isisisis = true;

        if (ANIM_STATE::AFTER_LOOP == iiii)
            iiii = ANIM_STATE::AFTER_END;
        else if (ANIM_STATE::BEFORE_LOOP == iiii)
            iiii = ANIM_STATE::BEFORE_END;
    }

    if (isisisis == true)
    {
        m_pModelCom->Set_Animation(iiii);
        if (ANIM_STATE::AFTER_LOOP == iiii || ANIM_STATE::AFTER_IDLE == iiii || ANIM_STATE::BEFORE_LOOP == iiii || ANIM_STATE::BEFORE_IDLE == iiii)
            m_pModelCom->Set_AnimationLoop(true);
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (ANIM_STATE::BEFORE_START == iiii)
        {
            m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_LOOP);
            m_pModelCom->Set_AnimationLoop(true);
            iiii = ANIM_STATE::BEFORE_LOOP;

            OutputDebugStringA("귀검 BEFORE_START 끝 / SET : BEFORE_LOOP\n");
        }
        if (ANIM_STATE::BEFORE_END == iiii)
        {
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
            m_pModelCom->Set_AnimationLoop(true);
            iiii = ANIM_STATE::AFTER_IDLE;

            OutputDebugStringA("귀검 BEFORE_END 끝 / SET : AFTER_IDLE\n");
        }
        if (ANIM_STATE::AFTER_START == iiii)
        {
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_LOOP);
            m_pModelCom->Set_AnimationLoop(true);
            iiii = ANIM_STATE::AFTER_LOOP;

            OutputDebugStringA("귀검 AFTER_START 끝 / SET : AFTER_LOOP\n");
        }
        if (ANIM_STATE::AFTER_END == iiii)
        {
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
            m_pModelCom->Set_AnimationLoop(true);
            iiii = ANIM_STATE::AFTER_IDLE;

            OutputDebugStringA("귀검 AFTER_END 끝 / SET : AFTER_IDLE\n");
        }
    }
}

void CBladeNexus::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this), );
}

HRESULT CBladeNexus::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

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

HRESULT CBladeNexus::Ready_Components(void* pArg)
{
    BLADENEXUS_DESC* pDesc = static_cast<BLADENEXUS_DESC*>(pArg);
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

CBladeNexus* CBladeNexus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBladeNexus* pInstance = new CBladeNexus(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBladeNexus::Clone(void* pArg)
{
    CBladeNexus* pInstance = new CBladeNexus(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBladeNexus::Free()
{
    __super::Free();


}
