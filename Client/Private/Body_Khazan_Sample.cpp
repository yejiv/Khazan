#include "Body_Khazan_Sample.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "MeshTrail.h"

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



    return S_OK;
}

void CBody_Khazan_Sample::Priority_Update(_float fTimeDelta)
{
    int a = 10;
    m_pTrail->Priority_Update(fTimeDelta);
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
        //if (Has_State(CKhazan_Sample::ATTACK_FAST))
        //    Remove_State(CKhazan_Sample::ATTACK_FAST);
        //if (Has_State(CKhazan_Sample::ATTACK_STRONG))
        //    Remove_State(CKhazan_Sample::ATTACK_STRONG);
        if (Has_State(CKhazan_Sample::ATTACK_FULLMOON))
            Remove_State(CKhazan_Sample::ATTACK_FULLMOON);
        if (Has_State(CKhazan_Sample::ATTACK_SHADOW1))
            Remove_State(CKhazan_Sample::ATTACK_SHADOW1);
        if (Has_State(CKhazan_Sample::ATTACK_SPIRAL))
            Remove_State(CKhazan_Sample::ATTACK_SPIRAL);
        if (Has_State(CKhazan_Sample::ATTACK_TWISTE))
            Remove_State(CKhazan_Sample::ATTACK_TWISTE);
        if (Has_State(CKhazan_Sample::ATTACK_STRIKE))
            Remove_State(CKhazan_Sample::ATTACK_STRIKE);
        if (Has_State(CKhazan_Sample::ATTACK_SOON))
            Remove_State(CKhazan_Sample::ATTACK_SOON);
        if (Has_State(CKhazan_Sample::ATTACK_VITALPOINT))
            Remove_State(CKhazan_Sample::ATTACK_VITALPOINT);
        if (Has_State(CKhazan_Sample::ATTACK_SHADOW2))
            Remove_State(CKhazan_Sample::ATTACK_SHADOW2);
        if (Has_State(CKhazan_Sample::ATTACK_BRUTAL))
            Remove_State(CKhazan_Sample::ATTACK_BRUTAL);
        if (Has_State(CKhazan_Sample::ATTACK_COUNT))
            Remove_State(CKhazan_Sample::ATTACK_COUNT);
    }

    if (isChanged)
    {
        if (Has_State(CKhazan_Sample::ATTACK_ALL))
        {

            //if(Has_State(CKhazan_Sample::ATTACK_FAST))
            //    m_pModelCom->Set_Animation(0);

            //m_isSetAnimation = false;
            //if (!m_isSetAnimation && Has_State(CKhazan_Sample::ATTACK_SET)) {
            //    m_pModelCom->Set_AnimationSet("Set1");
            //    m_isSetAnimation = true;
            //}

        }
        else
        {
			if (isChanged && Has_State(CKhazan_Sample::IDLE))
			{
                m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Stand"));
            }
			if (isChanged && Has_State(CKhazan_Sample::WALK))
			{
                m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_F"));
			}
			if (isChanged && Has_State(CKhazan_Sample::RUN))
			{
                m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_F"));
			}
        }

    }

    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);


    Update_CombinedMatrix();

    m_pTrail->Update(fTimeDelta);
}

void CBody_Khazan_Sample::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::OUTLINE, this)))
        return;

    m_pTrail->Late_Update(fTimeDelta);

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

    m_pTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), nullptr));

    return S_OK;
}

HRESULT CBody_Khazan_Sample::Ready_AnimationEvent()
{
    m_pModelCom->Register_Event("e1", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect1_Enter(); });
    m_pModelCom->Register_Event("e1", ANIM_EVENT_TRIGGERTYPE::EXIT,        [this]() {Effect1_Exit(); });
    m_pModelCom->Register_Event("e1", ANIM_EVENT_TRIGGERTYPE::CONTINUE,    [this]() {Effect1_Continue(); });

    m_pModelCom->Register_Event("e2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect2_Enter(); }); 
    m_pModelCom->Register_Event("e2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect2_Exit(); });
    m_pModelCom->Register_Event("e2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect2_Continue(); });

    m_pModelCom->Register_Event("e3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect3_Enter(); });
    m_pModelCom->Register_Event("e3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect3_Exit(); });
    m_pModelCom->Register_Event("e3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect3_Continue(); });

    m_pModelCom->Register_Event("e4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect4_Enter(); }); 
    m_pModelCom->Register_Event("e4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect4_Exit(); });
    m_pModelCom->Register_Event("e4", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect4_Continue(); });

    m_pModelCom->Register_Event("e5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect5_Enter(); });
    m_pModelCom->Register_Event("e5", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect5_Exit(); });
    m_pModelCom->Register_Event("e5", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect5_Continue(); });

    m_pModelCom->Register_Event("e6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect6_Enter(); });
    m_pModelCom->Register_Event("e6", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect6_Exit(); });
    m_pModelCom->Register_Event("e6", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect6_Continue(); });

    m_pModelCom->Register_Event("e7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect7_Enter(); });
    m_pModelCom->Register_Event("e7", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect7_Exit(); });
    m_pModelCom->Register_Event("e7", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect7_Continue(); });

    m_pModelCom->Register_Event("e8", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect8_Enter(); });
    m_pModelCom->Register_Event("e8", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect8_Exit(); });
    m_pModelCom->Register_Event("e8", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect8_Continue(); });

    m_pModelCom->Register_Event("e9", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()       {Effect9_Enter(); });
    m_pModelCom->Register_Event("e9", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()        {Effect9_Exit(); });
    m_pModelCom->Register_Event("e9", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()    {Effect9_Continue(); });

    m_pModelCom->Register_Event("e10", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect10_Enter(); });
    m_pModelCom->Register_Event("e10", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect10_Exit(); });
    m_pModelCom->Register_Event("e10", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect10_Continue(); });

    m_pModelCom->Register_Event("e11", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect11_Enter(); }); 
    m_pModelCom->Register_Event("e11", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect11_Exit(); });
    m_pModelCom->Register_Event("e11", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect11_Continue(); });

    m_pModelCom->Register_Event("e12", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect12_Enter(); });
    m_pModelCom->Register_Event("e12", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect12_Exit(); });
    m_pModelCom->Register_Event("e12", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect12_Continue(); });

    m_pModelCom->Register_Event("e13", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect13_Enter(); });
    m_pModelCom->Register_Event("e13", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect13_Exit(); });
    m_pModelCom->Register_Event("e13", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect13_Continue(); });

    m_pModelCom->Register_Event("e14", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect14_Enter(); });
    m_pModelCom->Register_Event("e14", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect14_Exit(); });
    m_pModelCom->Register_Event("e14", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect14_Continue(); });

    m_pModelCom->Register_Event("e15", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()      {Effect15_Enter(); });
    m_pModelCom->Register_Event("e15", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()       {Effect15_Exit(); });
    m_pModelCom->Register_Event("e15", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]()   {Effect15_Continue(); });



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

    int a = 0;

}

void CBody_Khazan_Sample::Effect1_Exit()
{
    //cout << "[Effect1_Exit]" << endl;
   // OutputDebugStringA("[Effect1_Exit] \n");
    int a = 0;
}

void CBody_Khazan_Sample::Effect1_Continue()
{
    //cout << "[Effect1_Continue]" << endl;
    //OutputDebugStringA("[Effect1_Continue] \n");


    _matrix tip = m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearFX_Matrix) * m_pParentTransform->Get_WorldMatrix();
    _matrix hand = m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearWeaponR_Matrix) * m_pParentTransform->Get_WorldMatrix();
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}

void CBody_Khazan_Sample::Effect2_Enter()
{
}

void CBody_Khazan_Sample::Effect2_Exit()
{
}

void CBody_Khazan_Sample::Effect2_Continue()
{
}

void CBody_Khazan_Sample::Effect3_Enter()
{
}

void CBody_Khazan_Sample::Effect3_Exit()
{
}

void CBody_Khazan_Sample::Effect3_Continue()
{
}

void CBody_Khazan_Sample::Effect4_Enter()
{
}

void CBody_Khazan_Sample::Effect4_Exit()
{
}

void CBody_Khazan_Sample::Effect4_Continue()
{
}

void CBody_Khazan_Sample::Effect5_Enter()
{
}

void CBody_Khazan_Sample::Effect5_Exit()
{
}

void CBody_Khazan_Sample::Effect5_Continue()
{
}

void CBody_Khazan_Sample::Effect6_Enter()
{
}

void CBody_Khazan_Sample::Effect6_Exit()
{
}

void CBody_Khazan_Sample::Effect6_Continue()
{
}

void CBody_Khazan_Sample::Effect7_Enter()
{
}

void CBody_Khazan_Sample::Effect7_Exit()
{
}

void CBody_Khazan_Sample::Effect7_Continue()
{
}

void CBody_Khazan_Sample::Effect8_Enter()
{
}

void CBody_Khazan_Sample::Effect8_Exit()
{
}

void CBody_Khazan_Sample::Effect8_Continue()
{
}

void CBody_Khazan_Sample::Effect9_Enter()
{
}

void CBody_Khazan_Sample::Effect9_Exit()
{
}

void CBody_Khazan_Sample::Effect9_Continue()
{
}

void CBody_Khazan_Sample::Effect10_Enter()
{
}

void CBody_Khazan_Sample::Effect10_Exit()
{
}

void CBody_Khazan_Sample::Effect10_Continue()
{
}

void CBody_Khazan_Sample::Effect11_Enter()
{
}

void CBody_Khazan_Sample::Effect11_Exit()
{
}

void CBody_Khazan_Sample::Effect11_Continue()
{
}

void CBody_Khazan_Sample::Effect12_Enter()
{
}

void CBody_Khazan_Sample::Effect12_Exit()
{
}

void CBody_Khazan_Sample::Effect12_Continue()
{
}

void CBody_Khazan_Sample::Effect13_Enter()
{
}

void CBody_Khazan_Sample::Effect13_Exit()
{
}

void CBody_Khazan_Sample::Effect13_Continue()
{
}

void CBody_Khazan_Sample::Effect14_Enter()
{
}

void CBody_Khazan_Sample::Effect14_Exit()
{
}

void CBody_Khazan_Sample::Effect14_Continue()
{
}

void CBody_Khazan_Sample::Effect15_Enter()
{
}

void CBody_Khazan_Sample::Effect15_Exit()
{
}

void CBody_Khazan_Sample::Effect15_Continue()
{
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
    Safe_Release(m_pTrail);


}
