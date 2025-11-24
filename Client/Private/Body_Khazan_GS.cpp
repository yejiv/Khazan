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

    if (FAILED(Ready_AnimationEvents()))
        return E_FAIL;

     /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

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
    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);

    Update_CombinedMatrix();

    Update_Colliders(fTimeDelta);

    m_pTrail->Update(fTimeDelta);

    Check_Guarding(fTimeDelta);

    Test_Attack(fTimeDelta);

    if (m_isCollision)
    {
        m_isCollision = false;
        //m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), XMLoadFloat4(&m_fCollisionPos));
    }
}

void CBody_Khazan_GS::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    m_pTrail->Late_Update(fTimeDelta);

}

HRESULT CBody_Khazan_GS::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    ///* After Image */
    //for (size_t i = 0; i < 10; i++)
    //{
    //    if (m_pModelCom->Restore_Frame(i))
    //    {
    //        // 본 행렬 바인딩 (복원된 상태로)
    //        _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    //        for (_uint j = 0; j < iNumMeshes; j++)
    //        {
    //            if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", j)))
    //                continue;
    //        }

    //        // 파츠 렌더링
    //        Render_Part(m_pModelCom_Arm);
    //        Render_Part(m_pModelCom_Face);
    //        Render_Part(m_pModelCom_Hair);
    //        Render_Part(m_pModelCom_Leg);
    //        Render_Part(m_pModelCom_Shoes);
    //        Render_Part(m_pModelCom_Torso);
    //    }
    //}

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

    Render_Part(m_pModelCom_Arm);
    Render_Part(m_pModelCom_Face);
    Render_Part(m_pModelCom_Hair);
    Render_Part(m_pModelCom_Leg);
    Render_Part(m_pModelCom_Shoes);
    Render_Part(m_pModelCom_Torso);

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

    Render_Part_Shadow(m_pModelCom_Arm);
    Render_Part_Shadow(m_pModelCom_Face);
    Render_Part_Shadow(m_pModelCom_Hair);
    Render_Part_Shadow(m_pModelCom_Leg);
    Render_Part_Shadow(m_pModelCom_Shoes);
    Render_Part_Shadow(m_pModelCom_Torso);

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

    Render_Part_Outline(m_pModelCom_Arm);
    Render_Part_Outline(m_pModelCom_Face);
    Render_Part_Outline(m_pModelCom_Hair);
    Render_Part_Outline(m_pModelCom_Leg);
    Render_Part_Outline(m_pModelCom_Shoes);
    Render_Part_Outline(m_pModelCom_Torso);

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

void CBody_Khazan_GS::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        /* 공격 콜라이더 */
        if (m_isSpearTipActive)
        {
            CCreature* pMonster = static_cast<CCreature*>(pDesc->pGameObject);
            if (pMonster == nullptr || pMonster->Get_CurrentHP() < 0.f)
                return;

            pMonster->Take_Damage(m_pPlayerData->fBonusDamage, static_cast<HITREACTION>(*m_pHitReaction), this);
            //pMonster->Take_Damage(m_pPlayerData->fDamage , static_cast<HITREACTION>(*m_pHitReaction), nullptr);
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 15.f, 50.f);
            m_isCollision = true;
            CTransform* MonsterTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));
            XMStoreFloat4(&m_fCollisionPos, MonsterTransform->Get_State(STATE::POSITION));
        }

        /*  탐지 */
        CGameObject* pObj = pDesc->pGameObject;
        if (!pObj || pObj->Get_IsDead()) return;
        if (pObj && (find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end()))
            m_CollMonsters.push_back(pObj);

    }

    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        /* 방어 콜라이더  */
        if (m_isSpearPoleActive)
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

}

void CBody_Khazan_GS::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CBody_Khazan_GS::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER)) {
        CGameObject* pObj = pDesc->pGameObject;

        if (!pObj) return;

        auto it = remove(m_CollMonsters.begin(), m_CollMonsters.end(), pObj);
        if (it != m_CollMonsters.end()) m_CollMonsters.erase(it, m_CollMonsters.end());

        if (m_CollMonsters.empty())
        {
            if (Has_Status(CKhazan_GSword::BRUTAL_BEGIN))
            {
                if (m_pBrutalAttack && !m_pBrutalAttack->Get_IsDead()) {
                    m_pBrutalAttack->Off_BrutalAttack();
                    // Safe_Release(m_pBrutalAttack);
                }

                // if (m_pBrutalmonster)
                     //Safe_Release(m_pBrutalmonster);
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

void CBody_Khazan_GS::Event_AttackTiming(_bool isAttackStart)
{
    if (isAttackStart)
    {
        m_isSpearTipActive = true;
        m_isSpearFullExtension = false;
        m_iCurSetAnimIndex = m_pModelCom->Get_CurAnimIndex();
        m_pBodyCom_Attack->Collision_Active(true);
        m_pBodyCom_BodyAttack->Collision_Active(true);
    }
    else
    {
        m_isSpearTipActive = false;
        m_isSpearFullExtension = true;
        // m_pBodyCom_Attack->Collision_Active(false);
        // m_pBodyCom_BodyAttack->Collision_Active(false);
    }
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
    m_pBodyCom_Search->Sync_Update(matParent);

}

void CBody_Khazan_GS::Check_Guarding(_float fTimeDelta)
{
    if (m_isSpearPoleActive)
        m_fJustGuardTime.x += fTimeDelta;

    if (*m_pIsGuarding == true && !m_isSpearPoleActive) {

        m_isSpearPoleActive = true;
        m_fJustGuardTime.x = 0.f;
        m_isJustGuardOnce = false;
    }
    if (*m_pIsGuarding == false && m_isSpearPoleActive) {

        m_isSpearPoleActive = false;
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

void CBody_Khazan_GS::Test_Attack(_float fTimeDelta)
{
    /* 약공 3타*/
    if(m_pGameInstance->Key_Down(DIK_1))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk01"));
    if (m_pGameInstance->Key_Pressing(DIK_Z,fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk02"));
    if (m_pGameInstance->Key_Pressing(DIK_X, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk03"));
    if (m_pGameInstance->Key_Pressing(DIK_C, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_SpiningCharger_01"));

    /* 차지 약공 */
    if (m_pGameInstance->Key_Down(DIK_2))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk01_Charge"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Gsword_WeakAtk01_ChargeAtk"));

    /* 차지 강공 */
    if (m_pGameInstance->Key_Down(DIK_3))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_StrongAtk01_Charge"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_StrongAtk03_Level0"));

    /* 스킬 : 거센기세 */
    if (m_pGameInstance->Key_Down(DIK_4))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk_FerociousMomentum"));

    /* 스킬 : 강기 발현  */
    if (m_pGameInstance->Key_Down(DIK_5))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_StrongAtk01_Charge_2"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_5))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_StrongAtk03_Level0"));


    /* 스킬 : 거인 사냥 , 귀신 */
    if (m_pGameInstance->Key_Down(DIK_6))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_SoulbringerGhostLiberation_Charge"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_6))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_SoulbringerGhostLiberation_ChargeAtk"));

    /* 스킬 : 정면 돌파*/
    if (m_pGameInstance->Key_Down(DIK_7))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeCrash_Charge"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_7))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeCrash_Atk"));


    /* 스킬 : 거대한 포효 */
    if (m_pGameInstance->Key_Down(DIK_9))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WarDeclaration"));
    if (m_pGameInstance->Key_Pressing(DIK_Z, fTimeDelta) && m_pGameInstance->Key_Down(DIK_9))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_WarDeclaration_Atk"));

    /* 스킬 : 거대한 포효 */
    if (m_pGameInstance->Key_Down(DIK_0))
        m_pModelCom->Set_Animation(m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeMaster_Frenzy_01"));


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

HRESULT CBody_Khazan_GS::Ready_Colliders()
{
    CBody::BODY_BOXSHAPE_DESC AttackDesc{};
    {
        AttackDesc.vExtent = _float3(1.2f, 0.7f, 0.7f);
        AttackDesc.eMotion = EMotionType::Kinematic;
        AttackDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        AttackDesc.eShapeType = SHAPE::BOX;
        AttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat4x4(&m_matWorldGSwordBody, XMLoadFloat4x4(m_pMatGSwordBody) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_matWorldGSwordBody));
        AttackDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        AttackDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        AttackDesc.vShapeOffset = _float3(-0.6f, 0.f, 0.f);
        m_tAttackCollisionDesc.pGameObject = this;
        m_tAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tAttackCollisionDesc.strName = TEXT("Player_Attack");
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
        m_tAttackCollisionDesc.pGameObject = this;
        m_tAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tAttackCollisionDesc.strName = TEXT("Player_Guard");
        GuardDesc.pCollisionDesc = &m_tAttackCollisionDesc;
        GuardDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body2"), reinterpret_cast<CComponent**>(&m_pBodyCom_Guard), &GuardDesc)))
            return E_FAIL;

    }

    CBody::BODY_SPHERESHAPE_DESC BodyAttackDesc{};
    {
        BodyAttackDesc.fRadius = 0.7f;
        BodyAttackDesc.bIsTrigger = true;
        BodyAttackDesc.bStartActive = true;
        BodyAttackDesc.eMotion = EMotionType::Kinematic;
        BodyAttackDesc.eQuality = EMotionQuality::Discrete;
        BodyAttackDesc.eShapeType = SHAPE::SPHERE;
        BodyAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&BodyAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&BodyAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        BodyAttackDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tBodyAttackCollisionDesc.pGameObject = this;
        m_tBodyAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tBodyAttackCollisionDesc.strName = TEXT("Player_Attack");
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
            TEXT("Com_Body4"), reinterpret_cast<CComponent**>(&m_pBodyCom_Search), &SearchDesc)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CBody_Khazan_GS::Ready_AnimationEvents()
{

#pragma region Effect

#pragma endregion

#pragma region Collider  
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Event_AttackTiming(true); });
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {Event_AttackTiming(false);  });
    m_pModelCom->Register_Event("GSwordOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pWSword->Set_Equipped(true);
        m_pClientInstance->Set_PlayerInput(true); });
    m_pModelCom->Register_Event("GSwordOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pWSword->Set_Equipped(false);
        m_pClientInstance->Set_PlayerInput(false); });
#pragma endregion

    return S_OK;
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

    Safe_Release(m_pBodyCom_BodyAttack);
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
