#include "E_Body_Khazan_Spear.h"
#include "E_Khazan_Spear.h"
#include "GameInstance.h"

CE_Body_Khazan_Spear::CE_Body_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CE_Body_Khazan_Spear::CE_Body_Khazan_Spear(const CE_Body_Khazan_Spear& Prototype)
    : CPartObject(Prototype)
{
}

_float4x4* CE_Body_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CE_Body_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CE_Body_Khazan_Spear::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SPEAR_DESC* pDesc = static_cast<BODY_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentStatus = pDesc->pStatus;
   // m_iCurState = *m_pParentState;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // m_pModelCom->Set_Animation(5);
     /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

    m_pModelCom->WarmupAnimations();

    // +		[12]	{strName="CA_P_Kazan_BareHands_Run_F" isLoop=true iDirection=1 ...}	Engine::tagAnimationSetUp
    //  m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Stand"));
    // +		[23]	{strName="CA_P_Kazan_BareHands_Walk_F" isLoop=true iDirection=1 ...}	Engine::tagAnimationSetUp
    // +		[211]	{strName="CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk" isLoop=false iDirection=0 ...}	Engine::tagAnimationSetUp

    m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_F"));

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));
    
    m_fRimPower = 5.f;
    m_fRimIntensity = 1.f;
    m_vRimColor = { 1.f, 1.f, 1.f };
    m_fCycleSpeed = 5.f;

    return S_OK;
}

void CE_Body_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
}

void CE_Body_Khazan_Spear::Update(_float fTimeDelta)
{
    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);

    Update_CombinedMatrix();
    XMStoreFloat4x4(&m_PrevCombinedMatrix, m_pTransformCom->Get_PrevWorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

    if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
    {
        m_isEnableMotionTrail = !m_isEnableMotionTrail;
        m_pMotionTrailCom->Set_Enable(m_isEnableMotionTrail);
    }
    
    m_pMotionTrailCom->Update(fTimeDelta);

    m_fBlinkTimeAcc += fTimeDelta;
}

void CE_Body_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::MOTIONVECTOR, this)))
    //      return;
}

HRESULT CE_Body_Khazan_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableEdge", &m_isEnableEdge)))
        return E_FAIL;

    _float fEdgeIntensity = 0.3f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    // Blink Rim Light Test
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &m_fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fBlinkTimeAcc, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCycleSpeed", &m_fCycleSpeed, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float3))))
        return E_FAIL;


    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    Render_Part(m_pModelCom_Arm);
    Render_Part(m_pModelCom_Face);
    Render_Part(m_pModelCom_Hair);
    Render_Part(m_pModelCom_Leg);
    Render_Part(m_pModelCom_Shoes);
    Render_Part(m_pModelCom_Torso);

    return S_OK;
}

HRESULT CE_Body_Khazan_Spear::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::PROJ))))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    Render_Part_Shadow(m_pModelCom_Arm);
    Render_Part_Shadow(m_pModelCom_Face);
    Render_Part_Shadow(m_pModelCom_Hair);
    Render_Part_Shadow(m_pModelCom_Leg);
    Render_Part_Shadow(m_pModelCom_Shoes);
    Render_Part_Shadow(m_pModelCom_Torso);

    return S_OK;
}

HRESULT CE_Body_Khazan_Spear::Render_MotionVector()
{
    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevCombinedMatrix)))
        return E_FAIL;
    
    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_PrevViewMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    
    if (FAILED(m_pMotionVectorShaderCom->Bind_Matrix("g_PrevProjMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};
    
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    _float2 vScreenSize = _float2(ViewportDesc.Width, ViewportDesc.Height);
    if (FAILED(m_pMotionVectorShaderCom->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_float2))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pMotionVectorShaderCom, "g_PrevBoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pMotionVectorShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    Render_Part_MotionVector(m_pModelCom_Arm);
    Render_Part_MotionVector(m_pModelCom_Face);
    Render_Part_MotionVector(m_pModelCom_Hair);
    Render_Part_MotionVector(m_pModelCom_Leg);
    Render_Part_MotionVector(m_pModelCom_Shoes);
    Render_Part_MotionVector(m_pModelCom_Torso);

    return S_OK;
}

void CE_Body_Khazan_Spear::Render_Part(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);

        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        //  m_pShaderCom->Begin(1);
        // Blink Rim Light Test
        m_pShaderCom->Begin(12);
        pModel->Render(i);
    }
}

void CE_Body_Khazan_Spear::Render_Part_Shadow(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i))) 
            continue;

        m_pShaderCom->Begin(2);
        pModel->Render(i);
    }
}

void CE_Body_Khazan_Spear::Render_Part_MotionVector(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_PrevBoneMatrices(m_pMotionVectorShaderCom, "g_PrevBoneMatrices", i)))
            continue;

        if (FAILED(pModel->Bind_BoneMatrices(m_pMotionVectorShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pMotionVectorShaderCom->Begin(0);
        pModel->Render(i);
    }
}

const MOTIONTRAIL_CONFIG& CE_Body_Khazan_Spear::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CE_Body_Khazan_Spear::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CE_Body_Khazan_Spear::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CE_Body_Khazan_Spear::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CE_Body_Khazan_Spear::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

HRESULT CE_Body_Khazan_Spear::Ready_Components()
{
   // LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh_MotionVector"),
        TEXT("Com_MotionVectorShader"), reinterpret_cast<CComponent**>(&m_pMotionVectorShaderCom), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Spear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
        TEXT("Com_Mode2"), reinterpret_cast<CComponent**>(&m_pModelCom_Arm), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
        TEXT("Com_Mode3"), reinterpret_cast<CComponent**>(&m_pModelCom_Face), nullptr)))
        return E_FAIL;
    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
    //    TEXT("Com_Mode4"), reinterpret_cast<CComponent**>(&m_pModelCom_Hair), nullptr)))
    //    return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
        TEXT("Com_Mode4"), reinterpret_cast<CComponent**>(&m_pModelCom_Hair), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
        TEXT("Com_Mode5"), reinterpret_cast<CComponent**>(&m_pModelCom_Leg), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
        TEXT("Com_Mode6"), reinterpret_cast<CComponent**>(&m_pModelCom_Shoes), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
        TEXT("Com_Mode7"), reinterpret_cast<CComponent**>(&m_pModelCom_Torso), nullptr)))
        return E_FAIL;

    m_pModelCom->Attach_Part(m_pModelCom_Arm);
    m_pModelCom->Attach_Part(m_pModelCom_Face);
    m_pModelCom->Attach_Part(m_pModelCom_Hair);
    m_pModelCom->Attach_Part(m_pModelCom_Leg);
    m_pModelCom->Attach_Part(m_pModelCom_Shoes);
    m_pModelCom->Attach_Part(m_pModelCom_Torso);

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = true;
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Arm);
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Face);
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Hair);
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Leg);
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Shoes);
    MTDesc.OwnerPartModels.push_back(m_pModelCom_Torso);
    MTDesc.Config.vLifeTime = { 0.f, 1.f };
    MTDesc.Config.vStartColor = { 1.f, 0.f, 1.f };
    MTDesc.Config.vTargetColor = { 0.f, 0.f, 1.f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 2.f;
    MTDesc.Config.isIndividualColor = true;
    MTDesc.Config.fColorUpdateSpeed = 1000.f;
    MTDesc.Config.fInterval = 0.2f;
    MTDesc.Config.iMaxFrames = 10.f;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CE_Body_Khazan_Spear::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CE_Body_Khazan_Spear* CE_Body_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CE_Body_Khazan_Spear* pInstance = new CE_Body_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CE_Body_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CE_Body_Khazan_Spear::Clone(void* pArg)
{
    CE_Body_Khazan_Spear* pInstance = new CE_Body_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CE_Body_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CE_Body_Khazan_Spear::Free()
{
    __super::Free();

    if (!m_isPrototype)
    {
        m_pModelCom->Detach_Part(m_pModelCom_Arm);
        m_pModelCom->Detach_Part(m_pModelCom_Face);
        m_pModelCom->Detach_Part(m_pModelCom_Hair);
        m_pModelCom->Detach_Part(m_pModelCom_Leg);
        m_pModelCom->Detach_Part(m_pModelCom_Shoes);
        m_pModelCom->Detach_Part(m_pModelCom_Torso);
    }

    Safe_Release(m_pParentTransform);
    Safe_Release(m_pMotionVectorShaderCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pModelCom_Torso);
    Safe_Release(m_pModelCom_Arm);
    Safe_Release(m_pModelCom_Face);
    Safe_Release(m_pModelCom_Hair);
    Safe_Release(m_pModelCom_Leg);
    Safe_Release(m_pModelCom_Shoes);
    Safe_Release(m_pModelCom);
}
