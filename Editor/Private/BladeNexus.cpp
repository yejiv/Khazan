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

    m_eAnimState = ANIM_STATE::BEFORE_IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_IDLE);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);

	return S_OK;
}

void CBladeNexus::Priority_Update(_float fTimeDelta)
{
    m_pModelCom->Set_AnimationBlend(true);
}

void CBladeNexus::Update(_float fTimeDelta)
{
    // BEFORE_IDLE > _START > _LOOP > _END >>                       // 첫 터치떄
    // >> AFTER_IDLE > _START > _LOOP > _END > AFTER_IDLE >>        // 반복

    // 7 > 9 > 8 > 9 > 8 > 9 >>> ...

    _bool isisisis = { false };

    this;

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
        else if (ANIM_STATE::BEFORE_LOOP == m_eAnimState)
            m_eAnimState = ANIM_STATE::BEFORE_END;
    }

    if (isisisis == true)
    {
        m_pModelCom->Set_Animation(m_eAnimState);
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState || ANIM_STATE::AFTER_IDLE == m_eAnimState ||
            ANIM_STATE::BEFORE_LOOP == m_eAnimState || ANIM_STATE::BEFORE_IDLE == m_eAnimState)
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
            m_eAnimState = ANIM_STATE::BEFORE_LOOP;
            m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_LOOP);
            m_pModelCom->Set_AnimationLoop(true);
        }
        if (ANIM_STATE::BEFORE_END == m_eAnimState)
        {
            // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
            m_eAnimState = ANIM_STATE::AFTER_IDLE;
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
            m_pModelCom->Set_AnimationLoop(true);
        }
        if (ANIM_STATE::AFTER_START == m_eAnimState)
        {
            // 다회 상호 작용 후 애니메이션 루프로 전환
            m_eAnimState = ANIM_STATE::AFTER_LOOP;
            m_pModelCom->Set_Animation(ANIM_STATE::AFTER_LOOP);
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

void CBladeNexus::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CBladeNexus::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fIntensity = 15.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fIntensity, sizeof(_float))))
        return E_FAIL;

    _float fRatio = 0.8f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorRatio", &fRatio, sizeof(_float))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        // 0 칼손잡이
        // 1 손 잘림 보호대
        // 2 뭐 존나 작은 눈
        // 3 밑에 작은 날카로운
        // 4 밑에 큰 날카로운
        // 5 눈
        //if (5 == i)
        //{
            _bool isEmissive = { false };
            _bool isSpecular = { false };
            _bool isMetalic = { false };
            _bool isRoughness = { false };

            // 0 == off, 1 == metal, 2 == rough, 3 == combined
            _int iTest = 0;

            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
                isEmissive = true;
            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0)))
                isSpecular = true;
            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS, 0)))
                isMetalic = true;
            if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_DIFFUSE_ROUGHNESS, 0)))
                isRoughness = true;

            m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isMetalic", &isMetalic, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isRoughness", &isRoughness, sizeof(_bool));
            //m_pShaderCom->Bind_RawValue("g_iTest", &iTest, sizeof(_int));
        //}

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(8), E_FAIL);

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

HRESULT CBladeNexus::Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

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
