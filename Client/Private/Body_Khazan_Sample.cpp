#include "Body_Khazan_Sample.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "XPBD.h"


static std::vector<int> MakeChainByNames(Engine::CModel* m,
    std::initializer_list<const char*> names) {
    std::vector<int> chain;
    chain.reserve(names.size());
    for (auto* n : names) {
        int idx = (_int)m->Get_BoneIndex(n);
        if (idx >= 0) chain.push_back(idx);
    }
    return chain;
}

CBody_Khazan_Sample::CBody_Khazan_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CBody_Khazan_Sample::CBody_Khazan_Sample(const CBody_Khazan_Sample& Prototype)
    : CPartObject{ Prototype }
{

}

_float4x4* CBody_Khazan_Sample::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



HRESULT CBody_Khazan_Sample::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_Sample::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SAMPLE_DESC* pDesc = static_cast<BODY_KHAZAN_SAMPLE_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_iCurState = *m_pParentState;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvent()))
        return E_FAIL;


    m_pModelCom->Set_Animation(5);
    /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

		ImGui::Begin("Sample Model State");

		m_pModelCom->Debug_RanderState();
		ImGui::End();
		});

    m_fEmissiveIntensity = 1.f;
#endif
    m_pModelCom->Set_Animation(5);
    m_pModelCom->Update_BoneCombinedMatrices();

    /*if (FAILED(Ready_BonePhysics()))
        return E_FAIL;*/

    return S_OK;
}

void CBody_Khazan_Sample::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CBody_Khazan_Sample::Update(_float fTimeDelta)
{
	_bool isChanged = false;

	if (m_iCurState != *m_pParentState)
	{
		m_iCurState = *m_pParentState;
        isChanged = true;
	}

    if (m_isFinishedAnimation)
    {
        if (Has_State(CKhazan_Sample::ATTACK_FAST))
            Remove_State(CKhazan_Sample::ATTACK_FAST);
        if (Has_State(CKhazan_Sample::ATTACK_SET))
            Remove_State(CKhazan_Sample::ATTACK_SET);
    }

    if (isChanged)
    {
        if (Has_State(CKhazan_Sample::ATTACK_ALL))
        {

            if(Has_State(CKhazan_Sample::ATTACK_FAST))
                m_pModelCom->Set_Animation(0);

            m_isSetAnimation = false;
            if (!m_isSetAnimation && Has_State(CKhazan_Sample::ATTACK_SET)) {
                m_pModelCom->Set_AnimationSet("Set1");
                m_isSetAnimation = true;
            }

        }
        else
        {
			if (isChanged && Has_State(CKhazan_Sample::IDLE))
			{
				m_pModelCom->Set_Animation(5);
			}
			if (isChanged && Has_State(CKhazan_Sample::WALK))
			{
				m_pModelCom->Set_Animation(6);
			}
			if (isChanged && Has_State(CKhazan_Sample::RUN))
			{
				m_pModelCom->Set_Animation(4);
			}
        }

    }

    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);

    //XMVECTOR vWindWS = XMVectorSet(0.f, 0.f , 1.f, 0.f);

    //m_pXPBD->PreUpdate(fTimeDelta, vWindWS);

    //m_pXPBD->Solve(fTimeDelta);

    //m_pXPBD->PostApply(fTimeDelta);


    Update_CombinedMatrix();
}

void CBody_Khazan_Sample::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::OUTLINE, this)))
        return;

#ifdef _DEBUG

#endif
}

HRESULT CBody_Khazan_Sample::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;



    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }



    return S_OK;
}

HRESULT CBody_Khazan_Sample::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_CurrentShadowLightViewMatrix())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_CurrentShadowLightProjMatrix())))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(2);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CBody_Khazan_Sample::Render_Outline()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutlineColor", &m_OutlineConfig.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutlineSize", &m_OutlineConfig.fSize, sizeof(_float))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        m_pShaderCom->Begin(3);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_Khazan_Sample::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Sample"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Khazan_Sample::Ready_AnimationEvent()
{
    m_pModelCom->Register_Event("0", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect1_Enter(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::EXIT,        [this]() {Effect1_Exit(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::CONTINUE,    [this]() {Effect1_Continue(); });
    m_pModelCom->Register_Event("Effect2", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect2(); });
    m_pModelCom->Register_Event("Effect3", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect3(); });
    m_pModelCom->Register_Event("Effect4", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect4(); });
    m_pModelCom->Register_Event("Effect5", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect5(); });
    m_pModelCom->Register_Event("Effect6", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect6(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect7_Enter(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::EXIT,        [this]() {Effect7_Exit(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::CONTINUE,    [this]() {Effect7_Continue(); });

    return S_OK;
}

HRESULT CBody_Khazan_Sample::Ready_BonePhysics()
{
    //vector<vector<_int>> vStrands;

    //vStrands.push_back({
    //    /*atic_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    stic_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),*/
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C1_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C5_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C2_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L3_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R2_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C4_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_005")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R3_006")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_DL_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_DL_002"))
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_UL_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_UL_002"))
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L2_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L2_002"))
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L1_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L1_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L1_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_L1_004"))
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_DR_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_DR_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_DR_003")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R1_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R1_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R1_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_R1_004")),
    //    });

    //vStrands.push_back({
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_BoneRoot_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C3_001")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C3_002")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C3_003")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C3_004")),
    //    static_cast<_int>(m_pModelCom->Get_BoneIndex("Hair_C3_005")),
    //    });

    //XPBD_Params params{};
    //params.fGravityY = -980.0f;

    //params.fDampingStill = 0.96f;
    //params.fDampingMove = 0.885f;

    //params.fStretchCompliance = 3.0e-5f;
    //params.fBendCompliance = 1.5e-4f;
    //params.fShearCompliance = 0.0f;

    //params.iSolverIterations = 6;
    //params.iSubsteps = 1;    // 진동 크면 2로

    //params.fTetherScale = 1.15f;
    //params.fTetherSoftness = 0.45f;

    //params.fSagBiasStill = 0.0f;
    //params.fSagBiasMove = 0.0f;

    //params.fAdvectPosRoot = 0.15f;
    //params.fAdvectPosTip = 0.35f;
    //params.fAdvectRotRoot = 0.15f;
    //params.fAdvectRotTip = 0.30f;

    //params.fAirLinear = 0.12f;
    //params.fAirQuadratic = 0.008f;
    //params.fMaxSpeed = 240.0f;

    //m_pXPBD = CXPBD::Create(m_pModelCom, m_pParentTransform, vStrands, params);

    return S_OK;
}

HRESULT CBody_Khazan_Sample::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


	return S_OK;
}

void CBody_Khazan_Sample::Effect1_Enter()
{
    char msg[256];
    sprintf_s(msg,"Local Bone SpearFX\n %.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n", 
        m_pSpearFX_Matrix->_11, m_pSpearFX_Matrix->_12, m_pSpearFX_Matrix->_13, m_pSpearFX_Matrix->_14,
        m_pSpearFX_Matrix->_21, m_pSpearFX_Matrix->_22, m_pSpearFX_Matrix->_23, m_pSpearFX_Matrix->_24,
        m_pSpearFX_Matrix->_31, m_pSpearFX_Matrix->_32, m_pSpearFX_Matrix->_33, m_pSpearFX_Matrix->_34,
        m_pSpearFX_Matrix->_41, m_pSpearFX_Matrix->_42, m_pSpearFX_Matrix->_43, m_pSpearFX_Matrix->_44);
    OutputDebugStringA(msg);

    _float4x4 matWorldSpearFX;
    XMStoreFloat4x4(&matWorldSpearFX, m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearFX_Matrix) * m_pParentTransform->Get_WorldMatrix());
    sprintf_s(msg, "Local Bone SpearFX\n %.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n",
        matWorldSpearFX._11, matWorldSpearFX._12, matWorldSpearFX._13, matWorldSpearFX._14,
        matWorldSpearFX._21, matWorldSpearFX._22, matWorldSpearFX._23, matWorldSpearFX._24,
        matWorldSpearFX._31, matWorldSpearFX._32, matWorldSpearFX._33, matWorldSpearFX._34,
        matWorldSpearFX._41, matWorldSpearFX._42, matWorldSpearFX._43, matWorldSpearFX._44);
    OutputDebugStringA(msg);
}

void CBody_Khazan_Sample::Effect1_Exit()
{
    //cout << "[Effect1_Exit]" << endl;
   // OutputDebugStringA("[Effect1_Exit] \n");

}

void CBody_Khazan_Sample::Effect1_Continue()
{
    //cout << "[Effect1_Continue]" << endl;
    //OutputDebugStringA("[Effect1_Continue] \n");

}

void CBody_Khazan_Sample::Effect2()
{
    /* OutputDebugStringA 원래 출력이 느립니다! 정확한 시점의 디버그는 cout으로 하십시오! */
    OutputDebugStringA("[Effect2] \n");

}

void CBody_Khazan_Sample::Effect3()
{
    OutputDebugStringA("[Effect3] \n");

}

void CBody_Khazan_Sample::Effect4()
{
    OutputDebugStringA("[Effect4] \n");

}

void CBody_Khazan_Sample::Effect5()
{
    OutputDebugStringA("[Effect5] \n");

}

void CBody_Khazan_Sample::Effect6()
{
    OutputDebugStringA("[Effect6] \n");

}

void CBody_Khazan_Sample::Effect7_Enter()
{
    OutputDebugStringA("[Effect7_Enter] \n");

}

void CBody_Khazan_Sample::Effect7_Exit()
{
    OutputDebugStringA("[Effect7_Exit] \n");

}

void CBody_Khazan_Sample::Effect7_Continue()
{
    OutputDebugStringA("[Effect7_Continue] \n");

}



inline _bool CBody_Khazan_Sample::Has_States()
{

	for (_uint i = 0; i < GetBitPosition(CKhazan_Sample::END); ++i)
	{
        if (Has_State(1 << i))
            return true;

	}
	return false;
}

CBody_Khazan_Sample* CBody_Khazan_Sample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_Sample* pInstance = new CBody_Khazan_Sample(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_Sample::Clone(void* pArg)
{
    CBody_Khazan_Sample* pInstance = new CBody_Khazan_Sample(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Khazan_Sample::Free()
{
    __super::Free();

    Safe_Release(m_pParentTransform);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    if (m_pXPBD) {
        Safe_Release(m_pXPBD);
    }

}
