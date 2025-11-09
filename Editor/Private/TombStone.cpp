#include "TombStone.h"

#include "GameInstance.h"

CTombStone::CTombStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CTombStone::CTombStone(const CTombStone& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CTombStone::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CTombStone::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::BEFORE_IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_IDLE);
    m_pModelCom->Set_AnimationLoop(true);

    return S_OK;
}

void CTombStone::Priority_Update(_float fTimeDelta)
{
}

void CTombStone::Update(_float fTimeDelta)
{
    // BEFORE_IDLE > BEFORE_START > AFTER_IDLE > AFTER_START > AFTER_LOOP > AFTER_END > AFTER_IDLE . . .

    _bool isisisis = { false };

    if (m_pGameInstance->Key_Down(DIK_7))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::BEFORE_START;
    }
    if (m_pGameInstance->Key_Down(DIK_8))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::AFTER_START;
    }
    if (m_pGameInstance->Key_Down(DIK_9))
    {
        isisisis = true;

        if (ANIM_STATE::AFTER_LOOP == m_eAnimState)
            m_eAnimState = ANIM_STATE::AFTER_END;
    }

    if (isisisis == true)
    {
        m_pModelCom->Set_Animation(m_eAnimState);
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState || ANIM_STATE::AFTER_IDLE == m_eAnimState || ANIM_STATE::BEFORE_IDLE == m_eAnimState)
        {
            // 활성화 후 작업 중일 때
            m_pModelCom->Set_AnimationLoop(true);
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (ANIM_STATE::BEFORE_START == m_eAnimState)
        {
            // 처음 상호 작용 후 애니메이션 루프로 전환
            m_eAnimState = ANIM_STATE::AFTER_IDLE;
            m_pModelCom->Set_Animation(m_eAnimState);
            m_pModelCom->Set_AnimationLoop(true);
        }
        if (ANIM_STATE::AFTER_START == m_eAnimState)
        {
            // 다회 상호 작용 후 애니메이션 루프로 전환
            m_eAnimState = ANIM_STATE::AFTER_LOOP;
            m_pModelCom->Set_Animation(m_eAnimState);
            m_pModelCom->Set_AnimationLoop(true);
        }
        if (ANIM_STATE::AFTER_END == m_eAnimState)
        {
            // 다회 상호 작용이 끝난 후 After Idle 상태로 전환
            m_eAnimState = ANIM_STATE::AFTER_IDLE;
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
            m_pModelCom->Set_AnimationLoop(true);
        }
    }
}

void CTombStone::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CTombStone::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        // 0 돌에 무슨 파란 무늬 블링블링 ( 얘는 Emmi 에는 Emmi 넣는게 맞고 )
        // 1 0번이랑은 다른 블링블링
        // 2 뭐임 얘는
        if (3 == i)
        {
            _bool isEmissive = { false };
            _bool isSpecular = { false };

            _bool isSpecToEmmi = true;

            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
                isEmissive = true;
            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0)))
                isSpecular = true;
            //if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_EMISSIVE, 0)))
            //    isEmissive = true;
            //if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_SPECULAR, 0)))
            //    isSpecular = true;

            m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isTest", &isSpecToEmmi, sizeof(_bool));
        }

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CTombStone::Ready_Components(void* pArg)
{
    TOMBSTONE_DESC* pDesc = static_cast<TOMBSTONE_DESC*>(pArg);
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

CTombStone* CTombStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTombStone* pInstance = new CTombStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTombStone::Clone(void* pArg)
{
    CTombStone* pInstance = new CTombStone(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTombStone::Free()
{
    __super::Free();


}
