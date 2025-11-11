#include "Body_Khazan_Spear.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "MeshTrail.h"

CBody_Khazan_Spear::CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CBody_Khazan_Spear::CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
{

}

_float4x4* CBody_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



HRESULT CBody_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_Spear::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SPEAR_DESC* pDesc = static_cast<BODY_KHAZAN_SPEAR_DESC*>(pArg);
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


    // m_pModelCom->Set_Animation(5);
     /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

    /* 뼈 행렬 가지고오기 */
    m_pSpearTip1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip");
    //m_pSpearTip2_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip_02");
    //m_pWeaponR_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R");
    //m_pSpearEnd1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_Spear_End01");
    //m_pSpearEnd2_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_Spear_End02");


    if (FAILED(Ready_Collider()))
        return E_FAIL;
    m_pSpearTip1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip");
    m_pWeaponR_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R");


//#ifdef _DEBUG
//	m_pGameInstance->AddWidget(TEXT("Client"), [this]() {
//
//		ImGui::Begin("Sample Model State");
//        if(m_pModelCom)
//		    m_pModelCom->Debug_RanderState();
//		ImGui::End();
//		});
//#endif

    return S_OK;
}

void CBody_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
    int a = 10;
    m_pTrail->Priority_Update(fTimeDelta);
}

void CBody_Khazan_Spear::Update(_float fTimeDelta)
{
    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);

    //m_pModelCom_Arm->Play_Animation(fTimeDelta);
    //m_pModelCom_Face->Play_Animation(fTimeDelta);
    //m_pModelCom_Hair->Play_Animation(fTimeDelta);
    //m_pModelCom_Shoes->Play_Animation(fTimeDelta);
    //m_pModelCom_Torso->Play_Animation(fTimeDelta);

    Update_CombinedMatrix();
    Update_Collider(fTimeDelta);

    XMStoreFloat4x4(&m_pSpearTip1_MatrixW, XMLoadFloat4x4(m_pSpearTip1_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
    XMStoreFloat4x4(&m_pWeaponR_MatrixW, XMLoadFloat4x4(m_pWeaponR_Matrix) * XMLoadFloat4x4(m_pParentMatrix));

    m_pTrail->Update(fTimeDelta);

    //TEST
    //if (m_pGameInstance->Key_Down(DIK_I))
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), XMVectorSet(1.f, 1.f, 1.f, 1.f));
    //if (m_pGameInstance->Key_Down(DIK_O))
    //    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"), XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

void CBody_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    // ========== Before Render ==========
    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
    //      return;
    //  //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
    //  //    return;
    // ========== After Render ==========
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;
    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::OUTLINE, this)))
    //      return;
#ifdef _DEBUG


#endif

    m_pTrail->Late_Update(fTimeDelta);

}

HRESULT CBody_Khazan_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        //  m_pShaderCom->Begin(1);

        //  m_pModelCom->Render(i);
    }

    Render_Part(m_pModelCom_Arm);
    Render_Part(m_pModelCom_Face);
    Render_Part(m_pModelCom_Hair);
    Render_Part(m_pModelCom_Leg);
    Render_Part(m_pModelCom_Shoes);
    Render_Part(m_pModelCom_Torso);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_Shadow()
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
    }

    Render_Part_Shadow(m_pModelCom_Arm);
    Render_Part_Shadow(m_pModelCom_Face);
    Render_Part_Shadow(m_pModelCom_Hair);
    Render_Part_Shadow(m_pModelCom_Leg);
    Render_Part_Shadow(m_pModelCom_Shoes);
    Render_Part_Shadow(m_pModelCom_Torso);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_Outline()
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
    }

    Render_Part_Outline(m_pModelCom_Arm);
    Render_Part_Outline(m_pModelCom_Face);
    Render_Part_Outline(m_pModelCom_Hair);
    Render_Part_Outline(m_pModelCom_Leg);
    Render_Part_Outline(m_pModelCom_Shoes);
    Render_Part_Outline(m_pModelCom_Torso);

    return S_OK;
}

void CBody_Khazan_Spear::Render_Part(CModel* pModel)
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

        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(1);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Render_Part_Shadow(CModel* pModel)
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

void CBody_Khazan_Spear::Render_Part_Outline(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(3);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        int a = 0;
    }
}

void CBody_Khazan_Spear::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        int a = 0;
    }
}

void CBody_Khazan_Spear::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

void CBody_Khazan_Spear::Update_Collider(_float fTimeDelta)
{
    const XMMATRIX matWorld = XMLoadFloat4x4(m_pSpearTip1_Matrix) * XMLoadFloat4x4(m_pParentMatrix);
    m_pBodyCom_SpearTip1->Sync_Update(matWorld);
    _vector vOutQuat, vOutPos;
    m_pBodyCom_SpearTip1->Update(fTimeDelta, matWorld, vOutQuat, vOutPos);
    XMStoreFloat4x4(&m_pSpearTip1_MatrixW, matWorld);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&m_pSpearTip1_MatrixW._41), vOutPos);


}

HRESULT CBody_Khazan_Spear::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Spear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
        TEXT("Com_Mode2"), reinterpret_cast<CComponent**>(&m_pModelCom_Arm), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
        TEXT("Com_Mode3"), reinterpret_cast<CComponent**>(&m_pModelCom_Face), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
        TEXT("Com_Mode4"), reinterpret_cast<CComponent**>(&m_pModelCom_Hair), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
        TEXT("Com_Mode5"), reinterpret_cast<CComponent**>(&m_pModelCom_Leg), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
        TEXT("Com_Mode6"), reinterpret_cast<CComponent**>(&m_pModelCom_Shoes), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
        TEXT("Com_Mode7"), reinterpret_cast<CComponent**>(&m_pModelCom_Torso), nullptr)))
        return E_FAIL;



    m_pModelCom->Attach_Part(m_pModelCom_Arm);
    m_pModelCom->Attach_Part(m_pModelCom_Face);
    m_pModelCom->Attach_Part(m_pModelCom_Hair);
    m_pModelCom->Attach_Part(m_pModelCom_Leg);
    m_pModelCom->Attach_Part(m_pModelCom_Shoes);
    m_pModelCom->Attach_Part(m_pModelCom_Torso);


    CMeshTrail::TRAIL_DESC MeshDsc;
    MeshDsc.iTextureIdx = 9;
    MeshDsc.fLifeTime = .25f;
    MeshDsc.iDivisionCount = 10.f;

    m_pTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDsc));

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_AnimationEvent()
{
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect1_Enter(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {Effect1_Exit(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Effect1_Continue(); });
    m_pModelCom->Register_Event("Effect2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect2(); });
    m_pModelCom->Register_Event("Effect3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect3(); });
    m_pModelCom->Register_Event("Effect4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect4(); });
    m_pModelCom->Register_Event("Effect5", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect5(); });
    m_pModelCom->Register_Event("Effect6", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect6(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Effect7_Enter(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {Effect7_Exit(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {Effect7_Continue(); });

    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk01_Trail(); });
    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk02_Trail(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk03_Trail(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });

    m_pModelCom->Register_Event("StrongAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_StrongAtk01_Trail(); });
    m_pModelCom->Register_Event("StrongAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_StrongAtk02_Trail(); });
    m_pModelCom->Register_Event("StrongAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_StrongAtk03_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_StrongAtk_Charge_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust(); });


    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_Collider()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    {
        BodyDesc.fRadius = 0.1f;
        BodyDesc.eMotion = EMotionType::Kinematic;
        BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        BodyDesc.eShapeType = SHAPE::SPHERE;
        BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK); // 추후에 Enum Monster attack 변경 할수도

        XMStoreFloat4x4(&m_pSpearTip1_MatrixW, XMLoadFloat4x4(m_pSpearTip1_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_pSpearTip1_MatrixW));
        BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tCollisionDesc.pGameObject = this;
        BodyDesc.pCollisionDesc = &m_tCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 10.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        BodyDesc.pCollisionDesc->pInfo = &DamageInfo;

        BodyDesc.bIsTrigger = true;
        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body1"), reinterpret_cast<CComponent**>(&m_pBodyCom_SpearTip1), &BodyDesc)))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CBody_Khazan_Spear::Effect1_Enter()
{
    char msg[256];
    sprintf_s(msg, "Local Bone SpearFX\n %.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n",
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

void CBody_Khazan_Spear::Effect1_Exit()
{
    //cout << "[Effect1_Exit]" << endl;
   // OutputDebugStringA("[Effect1_Exit] \n");

}

void CBody_Khazan_Spear::Effect1_Continue()
{
    //cout << "[Effect1_Continue]" << endl;
    //OutputDebugStringA("[Effect1_Continue] \n");

}

void CBody_Khazan_Spear::Effect2()
{
    /* OutputDebugStringA 원래 출력이 느립니다! 정확한 시점의 디버그는 cout으로 하십시오! */
    OutputDebugStringA("[Effect2] \n");

}

void CBody_Khazan_Spear::Effect3()
{
    OutputDebugStringA("[Effect3] \n");

}

void CBody_Khazan_Spear::Effect4()
{
    OutputDebugStringA("[Effect4] \n");

}

void CBody_Khazan_Spear::Effect5()
{
    OutputDebugStringA("[Effect5] \n");

}

void CBody_Khazan_Spear::Effect6()
{
    OutputDebugStringA("[Effect6] \n");

}

void CBody_Khazan_Spear::Effect7_Enter()
{
    OutputDebugStringA("[Effect7_Enter] \n");

}

void CBody_Khazan_Spear::Effect7_Exit()
{
    OutputDebugStringA("[Effect7_Exit] \n");

}

void CBody_Khazan_Spear::Effect7_Continue()
{
    OutputDebugStringA("[Effect7_Continue] \n");

}

void CBody_Khazan_Spear::FX_StrongAtk01_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}

void CBody_Khazan_Spear::FX_StrongAtk02_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}

void CBody_Khazan_Spear::FX_StrongAtk03_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}

void CBody_Khazan_Spear::FX_FastAtk01_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);

    //테스트용............
    _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);

    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {

        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
    m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), EffectID_SpearWind, Q, W.r[3]);
}

void CBody_Khazan_Spear::FX_FastAtk02_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);

    //테스트용............
    _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);

    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {

        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
   m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), EffectID_SpearWind, Q, W.r[3]);
}

void CBody_Khazan_Spear::FX_FastAtk03_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);

    //테스트용............
    _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);

    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {

        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
   m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), EffectID_SpearWind, Q, W.r[3]);
}

void CBody_Khazan_Spear::FX_FastAtk_SpawnWind()
{
    _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);

    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {

        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }
    EffectID_SpearWind = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), Q, W.r[3]); 
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust()
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"), m_pParentTransform->Get_WorldMatrix().r[3]);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pWeaponR_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}
 

inline _bool CBody_Khazan_Spear::Has_States()
{

    for (_uint i = 0; i < GetBitPosition(CKhazan_Sample::END); ++i)
    {
        if (Has_State(1 << i))
            return true;

    }
    return false;
}

CBody_Khazan_Spear* CBody_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_Spear::Clone(void* pArg)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Khazan_Spear::Free()
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
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pModelCom_Torso);
    Safe_Release(m_pModelCom_Arm);
    Safe_Release(m_pModelCom_Face);
    Safe_Release(m_pModelCom_Hair);
    Safe_Release(m_pModelCom_Leg);
    Safe_Release(m_pModelCom_Shoes);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTrail);
}
