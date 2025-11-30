#include "Body_Khazan_GS.h"
#include "Khazan_GSword.h"
#include "GSword_Khazan_GS.h"

#include "GameInstance.h"
#include "ClientInstance.h"

#include "Damage_Text.h"
#include "MeshTrail.h"
#include "Target_BrutalAttack.h"


CBody_Khazan_GS::CBody_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Khazan_GS::CBody_Khazan_GS(const CBody_Khazan_GS& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);

}
HRESULT CBody_Khazan_GS::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_GS::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_GS_DESC* pDesc = static_cast<BODY_KHAZAN_GS_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentStatus = pDesc->pStatus;
    m_pHitReaction = pDesc->pHitReation;
    m_iCurState = *m_pParentState;
    m_pGuardRotationTarget = pDesc->pGuardRotationTarget;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

    if (FAILED(Initialize_Equipment()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvents()))
        return E_FAIL;

    /* 뼈 행렬 가지고오기 */
    m_pMatGSwordBody = m_pModelCom->Get_BoneMatrix("FX_R_GSword_02");
    m_pMatGSwordTip = m_pModelCom->Get_BoneMatrix("FX_Weapon_R_Gsword_End");


    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
    m_isCollision = false;


    m_pModelCom->WarmupAnimations();

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));

    m_Offset_Matrix = XMMatrixRotationX(XMConvertToRadians(-90));

    return S_OK;
}

void CBody_Khazan_GS::Priority_Update(_float fTimeDelta)
{
    m_pTrail->Priority_Update(fTimeDelta);
}

void CBody_Khazan_GS::Update(_float fTimeDelta)
{
    m_isFinishedAnimation = m_pModelCom->Play_Animation(m_isNotifyAttacking ? fTimeDelta * 1.2f : fTimeDelta);

    Update_CombinedMatrix();

    Update_Colliders(fTimeDelta);

    m_pTrail->Update(fTimeDelta);

    Check_Guarding(fTimeDelta);

   // Test_Attack(fTimeDelta);

    if (m_isCollision)
    {
        m_isCollision = false;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), XMLoadFloat4(&m_fCollisionPos));
    }

    m_pMotionTrailCom->Update(fTimeDelta);
    if(m_isActiveMotionTrail)
       m_pMotionTrailCom->Start_MotionTrail(fTimeDelta);

    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        Trigger_MotionTrail(TEXT("MT_Common_WhiteDefault"), true);
    if (m_pGameInstance->Key_Pressing(DIK_X, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        Trigger_MotionTrail(TEXT("MT_Common_WhiteDefault"), false);
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
        Trigger_MotionTrail(TEXT("MT_Common_YellowDefualt"), true);
    if (m_pGameInstance->Key_Pressing(DIK_X, fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
        Trigger_MotionTrail(TEXT("MT_Common_YellowDefualt"), false);
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
        Trigger_MotionTrail(TEXT("MT_Common_RedDefault"), true);
    if (m_pGameInstance->Key_Pressing(DIK_X, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
        Trigger_MotionTrail(TEXT("MT_Common_RedDefault"), false);
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_4))
        Trigger_MotionTrail(TEXT("MT_Common_Avoid"), true);
    if (m_pGameInstance->Key_Pressing(DIK_X, fTimeDelta) && m_pGameInstance->Key_Down(DIK_4))
        Trigger_MotionTrail(TEXT("MT_Common_Avoid"), false);


    bool a =  m_pClientInstance->Is_CurrentSpear();
    bool b = m_pClientInstance->Is_CurrentGSword();

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT,fTimeDelta) && m_pGameInstance->Key_Down(DIK_M)){
        m_pClientInstance->Lock_Skill((1 << 4) );
        m_pClientInstance->Lock_Skill((1 << 8));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), m_pParentTransform->Get_State(STATE::POSITION));
    }

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_N)){
        m_pClientInstance->Lock_Skill((1 << 4));
        m_pClientInstance->Lock_Skill((1 << 8));
        m_pClientInstance->Unlock_Skill(1 << 4);
    }

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_B)){
        m_pClientInstance->Lock_Skill((1 << 4));
        m_pClientInstance->Lock_Skill((1 << 8));
        m_pClientInstance->Unlock_Skill(1 << 8);
    }

}

void CBody_Khazan_GS::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    m_pTrail->Late_Update(fTimeDelta);

    //test
    //if (m_pGameInstance->Key_Down(DIK_L) && m_pGameInstance->Key_Down(DIK_K))
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
}

HRESULT CBody_Khazan_GS::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableEdge", &m_isEnableEdge)))
        return E_FAIL;

    _float fEdgeIntensity = 0.3f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    _uint    iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_GS::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::PROJ))))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Shadow(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_GS::Render_Outline()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutlineColor", &m_OutlineConfig.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutlineSize", &m_OutlineConfig.fSize, sizeof(_float))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Outline(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_GS::Render_MotionVector()
{
    // 이전 프레임 월드, 뷰, 투영 바인드 해줘야 함
    if (FAILED(m_pTransformCom->Bind_PrevWorldMatrix(m_pShaderCom, "g_PrevWorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    _float2 vScreenSize = _float2(ViewportDesc.Width, ViewportDesc.Height);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_float2))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part_MotionVector(pModel);


    return S_OK;
}

void CBody_Khazan_GS::Render_Part(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);


        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(1);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_Shadow(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(2);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_Outline(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(3);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_MotionVector(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", i)))
            continue;

        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        // 셰이더 바꿔야 함
        //  m_pShaderCom->Begin(11);
        pModel->Render(i);

    }
}

void CBody_Khazan_GS::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        /* 방어 콜라이더  */
        if (m_isCollGuard_Active)
        {
            *m_pParentStatus |= CKhazan_GSword::GUARD;

            /* 저스트 가드 타이밍 */
            if (!m_isJustGuardOnce && m_fJustGuardTime.x <= m_fJustGuardTime.y) {
                *m_pParentStatus |= CKhazan_GSword::JUST_GUARD;
                m_isJustGuardOnce = true;
            }

            /* 가드후 충돌되면 충돌된 지점 봐라보게*/
            Start_GuardRotation(vContactPoint);
        }
    }


    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        CCreature* pMonster = static_cast<CCreature*>(pDesc->pGameObject);
        if (pMonster == nullptr || pMonster->Get_CurrentHP() < 0.f)
            return;

        /*  탐지 */
        if (pMyDesc->strName == TEXT("Player_Search"))
        {
            CGameObject* pObj = pDesc->pGameObject;
            if (!pObj || pObj->Get_IsDead()) return;
            lock_guard<mutex> lock(m_CollMonsterMutex);
            if (pObj && (find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end()))
                m_CollMonsters.push_back(pObj);
            return;
        }

       /* 공격 콜라이더 */
        _bool   isAttack = false;
        if (pMyDesc->strName == TEXT("Player_Attack1"))
        {
            isAttack = true;
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 15.f, 50.f);
            pMonster->Consume_Stamina(20.f);
        }

        if (pMyDesc->strName == TEXT("Player_RangeAttack"))
        {
            isAttack = true;
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 10.f, 45.f);
            pMonster->Consume_Stamina(15.f);
        }

        if (pMyDesc->strName == TEXT("Player_BodyAttack"))
        {
            isAttack = true;
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 25.f, 50.f);
            pMonster->Consume_Stamina(10.f);
        }

        if (isAttack)
        {
            pMonster->Take_Damage(m_pPlayerData->fBonusDamage, static_cast<HITREACTION>(*m_pHitReaction), this);
            m_isCollision = true;
            CTransform* MonsterTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));
            XMStoreFloat4(&m_fCollisionPos, MonsterTransform->Get_State(STATE::POSITION));
        }
    }

 

}

void CBody_Khazan_GS::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Khazan_GS::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER)) {
        CGameObject* pObj = pDesc->pGameObject;

        if (!pObj) return;
        lock_guard<mutex> lock(m_CollMonsterMutex);
        auto it = remove(m_CollMonsters.begin(), m_CollMonsters.end(), pObj);
        if (it != m_CollMonsters.end()) m_CollMonsters.erase(it, m_CollMonsters.end());

        if (m_CollMonsters.empty())
        {
            if (Has_Status(CKhazan_GSword::BRUTAL_BEGIN))
            {
                if (m_pBrutalAttack && !m_pBrutalAttack->Get_IsDead()) {
                    m_pBrutalAttack->Off_BrutalAttack();
                }

                Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
            }
        }
    }
}

_float4x4* CBody_Khazan_GS::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);

}

void CBody_Khazan_GS::Set_GSword(CGSword_Khazan_GS* pGS)
{
    m_pWSword = pGS; Safe_AddRef(m_pWSword);
}

void CBody_Khazan_GS::Search_BrutalTarget(_float fTimeDelta)
{
    /* 브루탈 개체는 하나만 */
    if (Has_Status(CKhazan_GSword::BRUTAL_BEGIN))
        return;

    m_fOptimizationSearchTime.x += fTimeDelta;

    if (m_fOptimizationSearchTime.x < m_fOptimizationSearchTime.y)
        return;

    m_fOptimizationSearchTime.x = 0.f;

    _vector vPlayerPos = XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f);
    lock_guard <mutex> lock(m_CollMonsterMutex);
    for (CGameObject* monster : m_CollMonsters)
    {
        if (!monster || monster->Get_IsDead())
            return;

        _vector vMonsterPos = monster->Get_Position();

        _vector  vDiff = vPlayerPos - vMonsterPos;
        _float  fDistSq = XMVectorGetX(XMVector3LengthSq(vDiff));

        /* 일정 범위에 다가가면  */
        if (fDistSq < 5.f * 5.f)
        {

            /* 후방 */
            _float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(monster->Get_Look()), XMVector3Normalize(vDiff)));
            if (fDot < 0.f)
            {

                m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
                m_pBrutalAttack->Setting_BrutalAttack(reinterpret_cast<const _float4*>(&monster->Get_Transform()->Get_WorldMatrixPtr()->_41), 0.f, { 0.f, 8.f });
                m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);

                m_pBrutalmonster = monster;
                m_isBackBrutal = true;
                m_isGroggyBrutal = false;

                Add_Status(CKhazan_GSword::BRUTAL_BEGIN);

                return;
            }

            /* 몬스터 그로기 상태*/
            CCreature* pCreatureMoster = static_cast<CCreature*>(monster);
            if (pCreatureMoster->Get_CurrentStamina() < 5.f)
            {
                m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
                m_pBrutalAttack->Setting_BrutalAttack(reinterpret_cast<const _float4*>(&monster->Get_Transform()->Get_WorldMatrixPtr()->_41), 5.f, { 0.f,8.f });
                m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
                m_pBrutalmonster = monster;

                m_isBackBrutal = false;
                m_isGroggyBrutal = true;

                Add_Status(CKhazan_GSword::BRUTAL_BEGIN);

                return;
            }

        }
    }
}

_bool CBody_Khazan_GS::Check_BrutalAttack(_float fTimeDelta)
{
    /* 범위 내에 브루탈 가능 개체가 없으면  */
    if (!Has_Status(CKhazan_GSword::BRUTAL_BEGIN))
        return false;

    /* 브루탈 어택 성공 후 아이콘 지우기 */
    if (Has_Status(CKhazan_GSword::BRUTAL_SUCCESS))
    {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        m_pBrutalAttack->Off_BrutalAttack();
        return false;
    }

    /* 몬스터가 죽으면  */
    if (!m_pBrutalmonster || m_pBrutalmonster->Get_IsDead()) {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        m_pBrutalAttack->Off_BrutalAttack();
        return false;
    }

    /* 브루탈 가능 시간이 다 되면 */
    if (m_pBrutalAttack->Get_IsDead()) {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        return false;
    }

    /* 브루탈 가능 범위인지 아닌지 체크 */
    _float  fDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f) - m_pBrutalmonster->Get_Position()));
    if (fDistSq < 4.f * 4.f) {
        if (!Has_Status(CKhazan_GSword::BRUTAL_READY)) {
            Add_Status(CKhazan_GSword::BRUTAL_READY);
            return true;
        }
    }
    else if (fDistSq > 4.f * 4.f + 1.f)
        if (Has_Status(CKhazan_GSword::BRUTAL_READY))
            Remove_Status(CKhazan_GSword::BRUTAL_READY);


    return false;
}

void CBody_Khazan_GS::AllAttackCollisionActive_Off()
{
    m_pBodyCom_Attack->Collision_Active(false);
    m_pBodyCom_RangeAttack->Collision_Active(false);
    m_pBodyCom_BodyAttack->Collision_Active(false);
    m_isNotifyAttacking = false;
}

void CBody_Khazan_GS::Event_AttackTiming(GS_COLLISION eColl, _bool isAttackStart)
{
   // m_iCurSetAnimIndex = m_pModelCom->Get_CurAnimIndex();

    if ((eColl & COLL_ATTACK) && isAttackStart)
        m_pBodyCom_Attack->Collision_Active(true);
    else  if ((eColl & COLL_ATTACK) && !isAttackStart)
        m_pBodyCom_Attack->Collision_Active(false);


    if ((eColl & COLL_RANGEATTACK) && isAttackStart)
        m_pBodyCom_RangeAttack->Collision_Active(true);
    else  if ((eColl & COLL_RANGEATTACK) && !isAttackStart)
        m_pBodyCom_RangeAttack->Collision_Active(false);


    if ((eColl & COLL_BODYATTACK) && isAttackStart)
        m_pBodyCom_BodyAttack->Collision_Active(true);
    else  if ((eColl & COLL_BODYATTACK) && !isAttackStart)
        m_pBodyCom_BodyAttack->Collision_Active(false);


    if ((eColl & COLL_GUARD) && isAttackStart)
        m_pBodyCom_Guard->Collision_Active(true);
    else  if ((eColl & COLL_GUARD) && !isAttackStart)
        m_pBodyCom_Guard->Collision_Active(false);

}

const MOTIONTRAIL_CONFIG& CBody_Khazan_GS::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CBody_Khazan_GS::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CBody_Khazan_GS::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CBody_Khazan_GS::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CBody_Khazan_GS::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

void CBody_Khazan_GS::Update_Colliders(_float fTimeDelta)
{
    _matrix matParent = XMLoadFloat4x4(m_pParentMatrix);
    _vector vOutQuat, vOutPos;

    const _matrix matWorld_GSwordTip = XMLoadFloat4x4(m_pMatGSwordTip) * matParent;
    m_pBodyCom_Attack->Sync_Update(matWorld_GSwordTip);
    m_pBodyCom_Attack->Update(fTimeDelta, matWorld_GSwordTip, vOutQuat, vOutPos);
    XMStoreFloat4x4(&m_matWorldGSwordTip, matWorld_GSwordTip);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_matWorldGSwordTip._41), vOutPos);

    _vector vOutQuat2, vOutPos2;
    const _matrix matWorld_GSwordBody = m_Offset_Matrix * XMLoadFloat4x4(m_pMatGSwordBody) * matParent;
    m_pBodyCom_Guard->Sync_Update(matWorld_GSwordBody);
    m_pBodyCom_Guard->Update(fTimeDelta, matWorld_GSwordBody, vOutQuat2, vOutPos2);
    XMStoreFloat4x4(&m_matWorldGSwordBody, matWorld_GSwordBody);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_matWorldGSwordBody._41), vOutPos2);

    m_pBodyCom_BodyAttack->Sync_Update(matParent);
    m_pBodyCom_RangeAttack->Sync_Update(matParent);
    m_pBodyCom_Search->Sync_Update(matParent);

}

void CBody_Khazan_GS::Check_Guarding(_float fTimeDelta)
{
    if (m_isCollGuard_Active)
        m_fJustGuardTime.x += fTimeDelta;

    if (*m_pIsGuarding == true && !m_isCollGuard_Active) {
        m_pBodyCom_Guard->Collision_Active(true);
        m_isCollGuard_Active = true;
        m_fJustGuardTime.x = 0.f;
        m_isJustGuardOnce = false;
    }
    if (*m_pIsGuarding == false && m_isCollGuard_Active) {

        m_pBodyCom_Guard->Collision_Active(false);
        m_isCollGuard_Active = false;
    }
}

void CBody_Khazan_GS::Update_GuardRotation(_float fTimeDelta)
{
    /* 부모에서 처리 */
}

void CBody_Khazan_GS::Start_GuardRotation(_float3 vContactPoint)
{
    // 1. 캐릭터의 위치
    _vector vCharacterPos = m_pParentTransform->Get_State(STATE::POSITION);

    // 2. 캐릭터 -> 접촉점 방향 벡터 계산
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;
    vHitDir = XMVectorSetY(vHitDir, 0.f); // XZ 평면으로 투영
    if (XMVectorGetX(XMVector3Length(vHitDir)) < 1e-4f)
    {
        vHitDir = m_pParentTransform->Get_State(STATE::LOOK);
        vHitDir = XMVectorSetY(vHitDir, 0.f);
        vHitDir = XMVector3Normalize(vHitDir);
    }
    else
        vHitDir = XMVector3Normalize(vHitDir);

    /* 부모에서 직접 회전 */
    *m_pParentStatus |= CKhazan_GSword::GUARD_ROTATION_REQUEST;
    XMStoreFloat4(m_pGuardRotationTarget, vHitDir);

}


void CBody_Khazan_GS::Exception_Animaition()
{
    //if(m_pModelCom->Get_CurAnimIndex() == 120 && *m_pModelCom->Get_CurTrackPosition() < 10.f)
    //{
    //    _vector vPos = m_pParentTransform->Get_State(STATE::POSITION);
    //    vPos.m128_f32[2] += 0.32f;
    //    m_pParentTransform->Set_State(STATE::POSITION, vPos);
    //
    //}
      
}

HRESULT CBody_Khazan_GS::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Khazan_GS::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_GSword"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    struct PartInfo {
        wstring strPartName;
        wstring strPrototype;
    };

    vector<PartInfo> partInfos = {
        /* Injured Set */
        { TEXT("Prisoner_Face1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1") },
        { TEXT("Prisoner_Hair1"), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair") },
        { TEXT("Prisoner_Torso1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3") },
        { TEXT("Prisoner_Arm1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1") },
        { TEXT("Prisoner_Leg3"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3") },
        { TEXT("Prisoner_Shoes1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1") },

        /* Thief Set */
        //{ TEXT("Thief_Head"), TEXT("Prototype_Component_Model_Khazan_Thief_Head") },
        //{ TEXT("Thief_Torso"), TEXT("Prototype_Component_Model_Khazan_Thief_Torso") },
        //{ TEXT("Thief_Arm"), TEXT("Prototype_Component_Model_Khazan_Thief_Arm") },
        //{ TEXT("Thief_Leg"), TEXT("Prototype_Component_Model_Khazan_Thief_Leg") },
        //{ TEXT("Thief_Shoes"), TEXT("Prototype_Component_Model_Khazan_Thief_Shoes") },
    };

    // 모든 파츠 로드
    for (const auto& info : partInfos)
    {
        CModel* pModel = nullptr;
        wstring strComponentTag = TEXT("Com_PartModel_") + info.strPartName;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), info.strPrototype,
            strComponentTag, reinterpret_cast<CComponent**>(&pModel), nullptr)))
            return E_FAIL;

        m_AllParts[info.strPartName] = pModel;
    }

    CMeshTrail::TRAIL_DESC MeshDsc;
    MeshDsc.iTextureIdx = 9;
    MeshDsc.fLifeTime = .25f;
    MeshDsc.iDivisionCount = 10.f;

    m_pTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDsc));

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = true;
    for (auto pModel : m_RenderParts)
        MTDesc.OwnerPartModels.push_back(pModel);
    MTDesc.Config.vLifeTime = { 0.f, 0.3f };
    MTDesc.Config.vStartColor = { 0.f, 0.f, 0.1f };
    MTDesc.Config.vTargetColor = { 0.5f, 1.f, 0.7f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 2.f;
    MTDesc.Config.isIndividualColor = true;
    MTDesc.Config.fColorUpdateSpeed = 1000.f;
    MTDesc.Config.fInterval = 0.05f;
    MTDesc.Config.iMaxFrames = 10.f;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
        return E_FAIL;

    return S_OK;

}

HRESULT CBody_Khazan_GS::Ready_Colliders()
{
    CBody::BODY_BOXSHAPE_DESC AttackDesc{};
    {
        AttackDesc.vExtent = _float3(0.7f, 0.7f, 1.8f);
        AttackDesc.eMotion = EMotionType::Kinematic;
        AttackDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        AttackDesc.eShapeType = SHAPE::BOX;
        AttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat4x4(&m_matWorldGSwordBody, XMLoadFloat4x4(m_pMatGSwordBody) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_matWorldGSwordBody));
        AttackDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        AttackDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        AttackDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tAttackCollisionDesc.pGameObject = this;
        m_tAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tAttackCollisionDesc.strName = TEXT("Player_Attack1");
        AttackDesc.pCollisionDesc = &m_tAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        AttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        AttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body1"), reinterpret_cast<CComponent**>(&m_pBodyCom_Attack), &AttackDesc)))
            return E_FAIL;

    }

    CBody::BODY_SPHERESHAPE_DESC RangeAttackDesc{};
    {
        RangeAttackDesc.fRadius = 4.5f;
        RangeAttackDesc.bIsTrigger = true;
        RangeAttackDesc.bStartActive = true;
        RangeAttackDesc.eMotion = EMotionType::Kinematic;
        RangeAttackDesc.eQuality = EMotionQuality::Discrete;
        RangeAttackDesc.eShapeType = SHAPE::SPHERE;
        RangeAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&RangeAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&RangeAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        RangeAttackDesc.vShapeOffset = _float3(0.f, -1.25f, 0.f);
        m_tRangeAttackCollisionDesc.pGameObject = this;
        m_tRangeAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tRangeAttackCollisionDesc.strName = TEXT("Player_RangeAttack");
        RangeAttackDesc.pCollisionDesc = &m_tRangeAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        RangeAttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        RangeAttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body2"), reinterpret_cast<CComponent**>(&m_pBodyCom_RangeAttack), &RangeAttackDesc)))
            return E_FAIL;

    }

    CBody::BODY_SPHERESHAPE_DESC BodyAttackDesc{};
    {
        BodyAttackDesc.fRadius = 1.f;
        BodyAttackDesc.bIsTrigger = true;
        BodyAttackDesc.bStartActive = true;
        BodyAttackDesc.eMotion = EMotionType::Kinematic;
        BodyAttackDesc.eQuality = EMotionQuality::Discrete;
        BodyAttackDesc.eShapeType = SHAPE::SPHERE;
        BodyAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&BodyAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&BodyAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        BodyAttackDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);
        m_tBodyAttackCollisionDesc.pGameObject = this;
        m_tBodyAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tBodyAttackCollisionDesc.strName = TEXT("Player_BodyAttack");
        BodyAttackDesc.pCollisionDesc = &m_tBodyAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        AttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        AttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body3"), reinterpret_cast<CComponent**>(&m_pBodyCom_BodyAttack), &BodyAttackDesc)))
            return E_FAIL;
    }

    CBody::BODY_BOXSHAPE_DESC GuardDesc{};
    {
        GuardDesc.vExtent = _float3(0.4f, 1.8f, 0.4f);
        GuardDesc.eMotion = EMotionType::Kinematic;
        GuardDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        GuardDesc.eShapeType = SHAPE::BOX;
        GuardDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat4x4(&m_matWorldGSwordBody, XMLoadFloat4x4(m_pMatGSwordBody) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_matWorldGSwordBody));
        GuardDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        GuardDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        GuardDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tGuardCollisionDesc.pGameObject = this;
        m_tGuardCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tGuardCollisionDesc.strName = TEXT("Player_Guard");
        GuardDesc.pCollisionDesc = &m_tGuardCollisionDesc;
        GuardDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body4"), reinterpret_cast<CComponent**>(&m_pBodyCom_Guard), &GuardDesc)))
            return E_FAIL;
    }

    CBody::BODY_SPHERESHAPE_DESC SearchDesc{};
    {
        SearchDesc.fRadius = 8.f;
        SearchDesc.bIsTrigger = true;
        SearchDesc.bStartActive = true;
        SearchDesc.eMotion = EMotionType::Kinematic;
        SearchDesc.eQuality = EMotionQuality::Discrete;
        SearchDesc.eShapeType = SHAPE::SPHERE;
        SearchDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);

        XMStoreFloat3(&SearchDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&SearchDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        SearchDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tSearchCollisionDesc.pGameObject = this;
        m_tSearchCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);
        m_tSearchCollisionDesc.strName = TEXT("Player_Search");
        SearchDesc.pCollisionDesc = &m_tSearchCollisionDesc;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body5"), reinterpret_cast<CComponent**>(&m_pBodyCom_Search), &SearchDesc)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CBody_Khazan_GS::Ready_AnimationEvents()
{ 
#pragma region Effect
    m_pModelCom->Register_Event("GS_WeakAtk01_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust"), m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_WeakAtk01_Charge_Ground", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("FerociousMomentum0"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody)); 
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]); 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]); 
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_BloodTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        }); 

    //강공 차지
    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust10"), m_pParentTransform->Get_State(STATE::POSITION)); //흰색
        }); 

    //강기발현
        //강기발현 차지
    m_pModelCom->Register_Event("GS_StrongAtk01_Force_Release_Charge", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust9"), m_pParentTransform->Get_State(STATE::POSITION));
        });

        //Strong Charge - 해금 X
    //m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현
            {
                _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail_V"), rot, m_pParentTransform->Get_State(STATE::POSITION));
            }
            //else if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::LIMIT_BREAK))    //한계극복
            //    ;
            else
                ; //그냥 차징 강공
        }
        else
            ; //그냥 차징 강공

        });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { 
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현 
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke_Red"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);  
            else
                ; // 그냥 차징 강공
        }
        else
            ; //그냥 차징 안한 강공
        });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현
            {
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Manifest_Strength_Land"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
            } 
            else
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GS_StrongATK"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        }
        else
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GS_StrongATK"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

    //거인사냥
    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody));
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);  
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle"), m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
        });

    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Giant_Hunt_Land"), XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        });

    m_pModelCom->Register_Event("GS_AsheFork_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        }); 

    //거대한 포효
    m_pModelCom->Register_Event("GS_WarDeclaration_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Giant_Roar"), m_pParentTransform->Get_State(STATE::POSITION));
        });

    //정면 돌파
    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_iFXIdx_BodyWind = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), m_iFXIdx_BodyWind, rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle"), m_pParentTransform->Get_State(STATE::POSITION));
        });

    //숨통끊기 선혈
    m_pModelCom->Register_Event("GS_GhostSlash_Trail1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_iFXIdx_Trail = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail1"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Trail2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail2"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail1"), m_iFXIdx_Trail, rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle2"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    // 내재된 분노 
    m_pModelCom->Register_Event("GS_RasingFurry_Explosion", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Inner_Range_Ground"), m_pParentTransform->Get_State(STATE::POSITION));
        });

    // 귀신 어둠의 그림자 
        //임시!
    m_pModelCom->Register_Event("GS_GhostLiberation_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle2"), m_pParentTransform->Get_State(STATE::POSITION));
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody));
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

    m_pModelCom->Register_Event("GS_GhostLiberation_Landing", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

    m_pModelCom->Register_Event("GS_GhostLiberation_Landing", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Ghost_Dark_Shadow_Land"), XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

    //한계극복
        //한계극복 차징
    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_Unlock_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust9"), m_pParentTransform->Get_State(STATE::POSITION)); //빨강!
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail_V"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke_Red"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Manifest_Strength_Land"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody).r[3]);
        });

#pragma endregion

#pragma region Collider  
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pBodyCom_Attack->Collision_Active(true); m_isNotifyAttacking = true; });
 //   m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { m_pBodyCom_Attack->Collision_Active(false); });

    m_pModelCom->Register_Event("RangeAttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pBodyCom_RangeAttack->Collision_Active(true); m_isNotifyAttacking = true; });
  //  m_pModelCom->Register_Event("RangeAttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()  { m_pBodyCom_RangeAttack->Collision_Active(false); });

    m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pBodyCom_BodyAttack->Collision_Active(true); m_isNotifyAttacking = true; });
  //  m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()  { m_pBodyCom_BodyAttack->Collision_Active(false); });

    m_pModelCom->Register_Event("WeaponOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  m_pWSword->Set_Equipped(true);  m_pClientInstance->Set_PlayerInput(true); });
    m_pModelCom->Register_Event("WeaponOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  m_pWSword->Set_Equipped(false); m_pClientInstance->Set_PlayerInput(false); });

    m_pModelCom->Register_Event("HEAL1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_pPlayerData->fCulHp += m_pPlayerData->fLachrymaItemRegen;
        if (m_pPlayerData->fCulHp > m_pPlayerData->fMaxHp)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp; }); //라크리마
    m_pModelCom->Register_Event("HEAL2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        
        m_pPlayerData->fCulHp += m_pPlayerData->fHealItemRegen;
        if (m_pPlayerData->fCulHp > m_pPlayerData->fMaxHp)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp; }); //힐템

#pragma endregion

    return S_OK;
} 

HRESULT CBody_Khazan_GS::Initialize_Equipment()
{
    const auto& equipData = m_pClientInstance->Get_PlayerEquipment();

    // 각 슬롯별 초기 장비 설정
    struct SlotMapping {
        EQUIPMENTTYPE type;
        _uint equipIndex;
    };

    vector<SlotMapping> slots = {
        { EQUIPMENTTYPE::HEAD, equipData.iHead },
        { EQUIPMENTTYPE::TORSO, equipData.iTorso },
        { EQUIPMENTTYPE::ARM, equipData.iArm },
        { EQUIPMENTTYPE::LEG, equipData.iLeg },
        { EQUIPMENTTYPE::SHOES, equipData.iShoes },
        { EQUIPMENTTYPE::FACE, equipData.iFace },
    };

    for (const auto& slot : slots)
    {
        if (slot.equipIndex != 0)
        {
            Equip_Part(slot.type, m_pClientInstance->Get_EquipmentName(slot.equipIndex));
        }
    }
    return S_OK;
}

void CBody_Khazan_GS::Equip_Part(EQUIPMENTTYPE eType, const _wstring& strPartName)
{
    if (eType == EQUIPMENTTYPE::SPEAR
        || eType == EQUIPMENTTYPE::GSWORD
        || eType == EQUIPMENTTYPE::NONE)
        return;

    // 기존 장착 파츠 해제
    auto iter = m_EquippedParts.find(eType);
    if (iter != m_EquippedParts.end())
    {
        auto partIter = m_AllParts.find(iter->second);
        if (partIter != m_AllParts.end())
        {
            m_pModelCom->Detach_Part(partIter->second);
            Safe_Release(partIter->second);
        }
        m_EquippedParts.erase(iter);
    }

    // 새 파츠 장착
    if (!strPartName.empty())
    {
        auto partIter = m_AllParts.find(strPartName);
        if (partIter != m_AllParts.end())
        {
            m_pModelCom->Attach_Part(partIter->second);
            m_EquippedParts[eType] = strPartName;
            Safe_AddRef(partIter->second);
        }
    }

    // 빠른 렌더용 
    Update_QuickRenderCache();
}

void CBody_Khazan_GS::Update_QuickRenderCache()
{
    for (auto pModel : m_RenderParts)
        Safe_Release(pModel);
    m_RenderParts.clear();

    /* 렌더 순서 적용 가능 */
    static const vector<EQUIPMENTTYPE> renderOrder = {
        EQUIPMENTTYPE::SHOES,
        EQUIPMENTTYPE::LEG,
        EQUIPMENTTYPE::TORSO,
        EQUIPMENTTYPE::ARM,
        EQUIPMENTTYPE::HEAD,
        EQUIPMENTTYPE::FACE,
    };

    for (EQUIPMENTTYPE type : renderOrder)
    {
        auto iter = m_EquippedParts.find(type);
        if (iter != m_EquippedParts.end())
        {
            auto partIter = m_AllParts.find(iter->second);
            if (partIter != m_AllParts.end())
            {
                m_RenderParts.push_back(partIter->second);
                Safe_AddRef(partIter->second);
            }
        }
    }

    /* todo !! 여기에 모션트레일컴포넌트에  랜더용 파츠모델 바꼈다고 넘겨주기. */
    
    // Part Model이 있는 경우!!
    m_pMotionTrailCom->Update_PartModels(m_RenderParts);

    // Part Model이 없고 Master Model만 있는 경우!! (무기)
    //  m_pMotionTrailCom->Update_MasterModel(m_pModelCom);
}

_vector CBody_Khazan_GS::Decompose_Rotation(_matrix W, _vector localRot, _vector offset)
{
    _vector S, Q, T;

    _matrix Local_Rotation = XMMatrixRotationQuaternion(localRot); 
    W = XMMatrixMultiply(Local_Rotation, W);

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
            offset
        );
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }

    return Q;
}

CBody_Khazan_GS* CBody_Khazan_GS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_GS* pInstance = new CBody_Khazan_GS(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_GS::Clone(void* pArg)
{
    CBody_Khazan_GS* pInstance = new CBody_Khazan_GS(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBody_Khazan_GS::Free()
{
    __super::Free();
    Safe_Release(m_pMotionTrailCom);

    Safe_Release(m_pBodyCom_BodyAttack);
    Safe_Release(m_pBodyCom_RangeAttack);
    Safe_Release(m_pBodyCom_Attack);
    Safe_Release(m_pBodyCom_Guard);
    Safe_Release(m_pBodyCom_Search);

    if (m_pBrutalAttack)
        Safe_Release(m_pBrutalAttack);
    if (m_pBrutalmonster)
        Safe_Release(m_pBrutalmonster);

    Safe_Release(m_pWSword);
    Safe_Release(m_pClientInstance);

    if (!m_isPrototype)
    {
        for (auto pModel : m_RenderParts)
            m_pModelCom->Detach_Part(pModel);
    }


    Safe_Release(m_pParentTransform);
    Safe_Release(m_pShaderCom);

    for (auto pModel : m_RenderParts)
        Safe_Release(pModel);
    m_RenderParts.clear();

    for (auto EquipPart : m_AllParts)
    {
        for (auto Part : m_EquippedParts)
        {
            if (Part.second == EquipPart.first)
            {
                Safe_Release(EquipPart.second);
                break;
            }
        }       
    }

    for (auto partIter : m_AllParts)
        Safe_Release(partIter.second);
    m_AllParts.clear();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pTrail);
}
