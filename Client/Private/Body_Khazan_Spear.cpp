#include "Body_Khazan_Spear.h"
#include "Khazan_Spear.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "MeshTrail.h"
#include "Spear_Khazan_Spear.h"
#include "Damage_Text.h"



CBody_Khazan_Spear::CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
 
{
}

CBody_Khazan_Spear::CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);

}

_float4x4* CBody_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



void CBody_Khazan_Spear::Set_Spear(CSpear_Khazan_Spear* pSpear)
{
    m_pSpear = pSpear; Safe_AddRef(m_pSpear);
}

HRESULT CBody_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_Spear::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SPEAR_DESC* pDesc = static_cast<BODY_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentStatus = pDesc->pStatus;
    m_pHitReaction = pDesc->pHitReation;
    m_iCurState = *m_pParentState;
    //m_pIsGuarding = pDesc->pIsGuarding;
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
   // m_LanternSocket_Matrix = m_pModelCom->Get_BoneMatrix("Lantern_Socket_L");
    //m_BackPack_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_C_BackPack_Spear");
    m_pSpearTip1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip");
    //m_pSpearTip2_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip_02");
    m_pSpearPole_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R");
    //m_pSpearEnd1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_Spear_End01");
    //m_pSpearEnd2_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_Spear_End02");


    if (FAILED(Ready_Collider()))
        return E_FAIL;

    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();

//#ifdef _DEBUG
//	m_pGameInstance->AddWidget(TEXT("Client"), [this]() {
//
//		ImGui::Begin("Sample Model State");
//        if(m_pModelCom)
//		    m_pModelCom->Debug_RanderState();
//		ImGui::End();
//		});
//#endif

    m_pModelCom->WarmupAnimations();

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));
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

    Update_CombinedMatrix();
    Update_Collider(fTimeDelta);

    m_pTrail->Update(fTimeDelta);

    Check_Guarding(fTimeDelta);
    Update_GuardRotation(fTimeDelta);

    //TEST
    if (m_pGameInstance->Key_Down(DIK_I))
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), XMVectorSet(1.f, 1.f, 1.f, 1.f) );
        //m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), m_pParentTransform->Get_WorldMatrix().r[3] );
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
        if (m_isSpearTipActive)
        {
            static_cast<CCreature*>(pDesc->pGameObject)->Take_Damage(m_pPlayerData->fDamage,static_cast<HITREACTION>(*m_pHitReaction), this);
        }

        if (m_isSpearPoleActive)
        {
            //*m_pParentStatus |= CKhazan_Spear::GUARD;

            /* 저스트 가드 타이밍 */
            if (m_fJustGuardTime.x <= m_fJustGuardTime.y)
                *m_pParentStatus |= CKhazan_Spear::JUST_GUARD;

            /* 가드후 충돌되면 충돌된 지점 봐라보게*/
            Start_GuardRotation(vContactPoint);
            // 테이크대미지 막기 
            // 회전 - 여기서 주기 
            // 성공 애니메이션 - ( 일단 스킵)
            // 저스트가드 -  부모 status 변경시켜주기 ,
            // 저스트가드가 성공했다는건? -> 프레임 계산
            // 가드가 지금 시작됐다는건? m_isSpearPoleActive true 타이밍 

        }

        

        int a = 0;

    }
}

void CBody_Khazan_Spear::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    //if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    //{
    //    int a = 0;
    //}
}

void CBody_Khazan_Spear::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

void CBody_Khazan_Spear::Update_Collider(_float fTimeDelta)
{
    _matrix matParent = XMLoadFloat4x4(m_pParentMatrix);
    _vector vOutQuat, vOutPos;

    const XMMATRIX matWorld_SpearTip1 = XMLoadFloat4x4(m_pSpearTip1_Matrix) * matParent;
    m_pBodyCom_SpearTip1->Sync_Update(matWorld_SpearTip1);
    m_pBodyCom_SpearTip1->Update(fTimeDelta, matWorld_SpearTip1, vOutQuat, vOutPos);
    XMStoreFloat4x4(&m_pSpearTip1_MatrixW, matWorld_SpearTip1);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_pSpearTip1_MatrixW._41), vOutPos);

    _vector vOutQuat2, vOutPos2;
    const XMMATRIX matWorld_SpearPole = m_SpearOffset_Matrix *  XMLoadFloat4x4(m_pSpearPole_Matrix) * matParent;
    m_pBodyCom_SpearPole->Sync_Update(matWorld_SpearPole);
    m_pBodyCom_SpearPole->Update(fTimeDelta, matWorld_SpearPole, vOutQuat2, vOutPos2);
    XMStoreFloat4x4(&m_pSpearPole_MatrixW, matWorld_SpearPole);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_pSpearPole_MatrixW._41), vOutPos2);
}

void CBody_Khazan_Spear::Check_Guarding(_float fTimeDelta)
{
    if (*m_pIsGuarding == true && !m_isSpearPoleActive) {

        m_isSpearPoleActive = true;
    }
    if (*m_pIsGuarding == false && m_isSpearPoleActive) {

        m_isSpearPoleActive = false;
    }

    if (m_isSpearPoleActive)
        m_fJustGuardTime.x += fTimeDelta;
    else
        m_fJustGuardTime.x = 0.f;
}

void CBody_Khazan_Spear::Update_GuardRotation(_float fTimeDelta)
{
    if (!m_isGuardRotating)
        return;

    m_fGuardRotationTime += fTimeDelta;

    // 회전 시간이 끝났으면 정확히 목표 방향으로 설정
    if (m_fGuardRotationTime >= m_fGuardRotationDuration)
    {
        m_isGuardRotating = false;

        // 정확히 목표 방향으로 설정
        _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_vTargetRotationDir));
        _vector vUp = XMVector3Normalize(XMVector3Cross(m_vTargetRotationDir, vRight));

        m_pParentTransform->Set_State(STATE::RIGHT, vRight);
        m_pParentTransform->Set_State(STATE::UP, vUp);
        m_pParentTransform->Set_State(STATE::LOOK, m_vTargetRotationDir);

        return;
    }

    // Ease-Out 보간 (처음엔 빠르게, 끝으로 갈수록 천천히)
    _float fRatio = m_fGuardRotationTime / m_fGuardRotationDuration;
    fRatio = 1.f - (1.f - fRatio) * (1.f - fRatio); // Ease-Out Quadratic

    // 현재 각도 계산
    _float fCurrentAngle = m_fStartAngle + (m_fTargetAngle - m_fStartAngle) * fRatio;

    // 방향 벡터 생성
    _vector vNewLook = XMVectorSet(cosf(fCurrentAngle), 0.f, sinf(fCurrentAngle), 0.f);
    vNewLook = XMVector3Normalize(vNewLook);

    // Right, Up 벡터 계산
    _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLook));
    _vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

    // Transform에 적용
    m_pParentTransform->Set_State(STATE::RIGHT, vRight);
    m_pParentTransform->Set_State(STATE::UP, vUp);
    m_pParentTransform->Set_State(STATE::LOOK, vNewLook);
}

void CBody_Khazan_Spear::Start_GuardRotation(_float3 vContactPoint)
{
    // 1. 캐릭터의 위치
    _vector vCharacterPos = m_pParentTransform->Get_State(STATE::POSITION);

    // 2. 캐릭터 -> 접촉점 방향 벡터 계산
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;
    vHitDir = XMVectorSetY(vHitDir, 0.f); // XZ 평면으로 투영
    vHitDir = XMVector3Normalize(vHitDir);

    // 3. 현재 캐릭터의 Forward 방향
    _vector vCurrentForward = m_pParentTransform->Get_State(STATE:: LOOK);
    vCurrentForward = XMVectorSetY(vCurrentForward, 0.f);
    vCurrentForward = XMVector3Normalize(vCurrentForward);

    // 4. 현재 각도와 목표 각도 계산
    m_fStartAngle = atan2f(XMVectorGetZ(vCurrentForward), XMVectorGetX(vCurrentForward));
    m_fTargetAngle = atan2f(XMVectorGetZ(vHitDir), XMVectorGetX(vHitDir));

    // 5. 최단 거리로 회전하도록 각도 보정 (-π ~ π)
    _float fAngleDiff = m_fTargetAngle - m_fStartAngle;
    if (fAngleDiff > XM_PI)
        fAngleDiff -= XM_2PI;
    else if (fAngleDiff < -XM_PI)
        fAngleDiff += XM_2PI;

    m_fTargetAngle = m_fStartAngle + fAngleDiff;

    // 6. 회전 시작
    m_isGuardRotating = true;
    m_fGuardRotationTime = 0.f;
    m_vTargetRotationDir = vHitDir;
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
    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
    //    TEXT("Com_Mode4"), reinterpret_cast<CComponent**>(&m_pModelCom_Hair), nullptr)))
    //    return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
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
#pragma region Effect
    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk01_Trail(); });
    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk02_Trail(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_FastAtk03_Trail(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_FastAtk_SpawnWind(); });

    m_pModelCom->Register_Event("StrongAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust1(m_pParentTransform->Get_WorldMatrix().r[3]); });

    /*보름달 트레일*/
    m_pModelCom->Register_Event("Full_Moon_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("Full_Moon_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    /*보름달 Blust*/
    m_pModelCom->Register_Event("Full_Moon_Spike0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust3(m_pParentTransform->Get_WorldMatrix().r[3]); });
    m_pModelCom->Register_Event("Full_Moon_Spike1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Spear_Spike(); });

    /*달빛 베기*/
    m_pModelCom->Register_Event("LightningSpear_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("LightningSpear_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("LightningSpear_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust6(m_pParentTransform->Get_WorldMatrix().r[3]); });


    /*나선 찌르기*/
    m_pModelCom->Register_Event("SpiralSpear_Spike0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //_matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
        //
        //_vector S, Q, T;
        //
        //if (!XMMatrixDecompose(&S, &Q, &T, W))
        //{
        //
        //    XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);
        //
        //
        //    _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        //    _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        //    _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));
        //
        //
        //    _matrix RotationMatrix(
        //        r0,
        //        r1,
        //        r2,
        //        XMVectorSet(0.f, 0.f, 0.f, 1.f)
        //    );
        //
        //    Q = XMQuaternionRotationMatrix(RotationMatrix);
        //}
        // EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiralSpear_SpearFX"), W.r[3]);
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        //_matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
        //
        //_vector S, Q, T;
        //
        //if (!XMMatrixDecompose(&S, &Q, &T, W))
        //{
        //
        //    XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);
        //
        //
        //    _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        //    _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        //    _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));
        //
        //
        //    _matrix RotationMatrix(
        //        r0,
        //        r1,
        //        r2,
        //        XMVectorSet(0.f, 0.f, 0.f, 1.f)
        //    );
        //
        //    Q = XMQuaternionRotationMatrix(RotationMatrix);
        //}
        //m_pGameInstance->Update_Effect_World(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiralSpear_SpearFX"), EffectID_SpiralSpear, Q, W.r[3]);

        });
    
    //m_pModelCom->Register_Event("SpiralSpear_Spike0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiralSpear_SpearFX"), EffectID_SpiralSpear); 
    //    });
    //m_pModelCom->Register_Event("SpiralSpear_Spike1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    });


#pragma endregion


#pragma region Collider  
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Event_AttackTiming(true); });
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {Event_AttackTiming(false);  });
    m_pModelCom->Register_Event("SpearOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pSpear->Set_Equipped(true);
        m_pClientInstance->Set_PlayerInput(true); });
    m_pModelCom->Register_Event("SpearOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pSpear->Set_Equipped(false);
        m_pClientInstance->Set_PlayerInput(false); });
#pragma endregion

   // m_pModelCom->Register_Event("LanternOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {*m_isEquipLantern = true; });
   // m_pModelCom->Register_Event("LanternOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { *m_isEquipLantern = false;  });




    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_Collider()
{
    CBody::BODY_BOXSHAPE_DESC TipBoxDesc{};
    {
        TipBoxDesc.vExtent = _float3(0.8f, 0.25f, 0.25f);
        TipBoxDesc.eMotion = EMotionType::Kinematic;
        TipBoxDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        TipBoxDesc.eShapeType = SHAPE::BOX;
        TipBoxDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK); // 추후에 Enum Monster attack 변경 할수도

        XMStoreFloat4x4(&m_pSpearTip1_MatrixW, XMLoadFloat4x4(m_pSpearTip1_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_pSpearTip1_MatrixW));
        TipBoxDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        TipBoxDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        TipBoxDesc.vShapeOffset = _float3(-0.4f, 0.f, 0.f);
        m_tCollisionDesc.pGameObject = this;
        TipBoxDesc.pCollisionDesc = &m_tCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 10.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        TipBoxDesc.pCollisionDesc->pInfo = &DamageInfo;

        TipBoxDesc.bIsTrigger = true;
        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body1"), reinterpret_cast<CComponent**>(&m_pBodyCom_SpearTip1), &TipBoxDesc)))
            return E_FAIL;

    }
    CBody::BODY_BOXSHAPE_DESC BodyBoxDesc{};
    {
        BodyBoxDesc.vExtent = _float3(0.1f, 1.8f, 0.1f);
        BodyBoxDesc.eMotion = EMotionType::Kinematic;
        BodyBoxDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        BodyBoxDesc.eShapeType = SHAPE::BOX;
        BodyBoxDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK); // 추후에 Enum Monster attack 변경 할수도

        XMStoreFloat4x4(&m_pSpearPole_MatrixW, XMLoadFloat4x4(m_pSpearPole_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_pSpearPole_MatrixW));
        BodyBoxDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        BodyBoxDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        BodyBoxDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tCollisionDesc.pGameObject = this;
        BodyBoxDesc.pCollisionDesc = &m_tCollisionDesc;
        BodyBoxDesc.bIsTrigger = true;
        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body2"), reinterpret_cast<CComponent**>(&m_pBodyCom_SpearPole), &BodyBoxDesc)))
            return E_FAIL; 
    }

    //m_pBodyCom_SpearTip1->Collision_Active(false);
    //m_pBodyCom_SpearPole->Collision_Active(false);
    
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

void CBody_Khazan_Spear::FX_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);
}

void CBody_Khazan_Spear::FX_FastAtk01_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW);
    _matrix hand = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
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
    _matrix hand = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
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
    _matrix hand = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
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

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.3f, 0.1f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}


void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust1(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"), pos);

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 0.5f;
    Desc.fPower = 0.03f;
    Desc.fDuration = 2.f;
    Desc.vFadeTime = _float2(1.f, 0.2f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 9;
    m_pGameInstance->Start_Distortion(Desc);

    // Vignette
    VIGNETTE_CONFIG Config{};
    Config.eMode = VIGNETTE_CONFIG::SMOOTH_SMOOTH;
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fIntensity = 1.f;
    Config.fMaxIntensity = 4.f;
    m_pGameInstance->Start_VignetteAnimation(2.f, Config);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust2(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust2"), pos);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust3(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust3"), pos);

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.02f;
    Desc.fDuration = 1.f;
    Desc.vFadeTime = _float2(0.4f, 0.2f);
    Desc.fSpeed = 1.5f;
    Desc.iNoiseIndex = 18;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust4(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust4"), pos);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust5(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust5"), pos); 
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust6(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust6"), pos);

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 0.5f;
    Desc.fPower = 0.03f;
    Desc.fDuration = 2.f;
    Desc.vFadeTime = _float2(1.f, 0.2f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 9;
    m_pGameInstance->Start_Distortion(Desc);

    // Vignette
    VIGNETTE_CONFIG Config{};
    Config.eMode = VIGNETTE_CONFIG::SMOOTH_SMOOTH;
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;       //범위
    Config.fIntensity = 1.f;    //현재 색상 강도
    Config.fMaxIntensity = 2.f; //최대 색상 강도
    m_pGameInstance->Start_VignetteAnimation(2.f, Config);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_BlustSmall(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BlustSmall"), pos);
}

void CBody_Khazan_Spear::Spear_Spike()
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

        T = W.r[3];
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }


    _vector V_LocalOffset = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
    _vector V_WorldOffset = XMVector3Rotate(V_LocalOffset, Q);
    _vector V_FinalPosition = T + V_WorldOffset;


    EffectID_SpearWind = m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust5"), Q, V_FinalPosition);
}

void CBody_Khazan_Spear::Event_AttackTiming(_bool isAttackStart)
{
    /* 공격 맞아서 중간에 끊길 경우는? */
    if (isAttackStart)
    {
        m_isSpearTipActive = true;
        m_isSpearFullExtension = false;
        m_iCurSetAnimIndex = m_pModelCom->Get_CurAnimIndex();
        m_pBodyCom_SpearTip1->Collision_Active(true);
       
    }
    else
    {
        m_isSpearTipActive = false;
        m_isSpearFullExtension = true;
        m_pBodyCom_SpearTip1->Collision_Active(false);
    }

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

    Safe_Release(m_pSpear);
    Safe_Release(m_pClientInstance);

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
