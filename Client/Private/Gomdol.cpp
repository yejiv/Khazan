#include "Gomdol.h"
#include "CharacterVirtual.h"
#include "Body_Gomdol.h"
#include "AI_Controller_Gomdol.h"
#include "Mon_HP.h"
#include "GameInstance.h"

CGomdol::CGomdol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{pDevice,pContext}
{
}

CGomdol::CGomdol(const CGomdol& Prototype)
    :CMonster{Prototype}
{
}

_float4* CGomdol::Get_LockOnPosition()
{
    return m_vLockOnPosition;
}

void CGomdol::Set_InitPosition()
{
    m_pCharVirCom->Set_Position(XMVectorSet(-4.f, 0.f, 17.78f, 1.f));
}

HRESULT CGomdol::Initialize_Prototype()
{
 
    return S_OK;
}

HRESULT CGomdol::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    if (FAILED(Ready_SFX()))
        return E_FAIL;

    m_pController = CAI_Controller_Gomdol::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if (nullptr != m_pController)
    {
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);
    }

    // HP 등록

    m_pTransformCom->Rotation(XMVectorSet(0.f,1.f,0.f,0.f),XMConvertToRadians(180.f));

    return S_OK;
}

void CGomdol::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CGomdol::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);


    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");

}

void CGomdol::Late_Update(_float fTimeDelta)
{
    if (!m_isDetected)
    {

        CBlackBoard* pBB = m_pController->Get_BlackBoard();
        if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
        {
            m_isDetected = true;

            m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));

            if (m_pUI_HP != nullptr)
            {
                m_pUI_HP->Setting_HP(m_vLockOnPosition, { 0.f, 80.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
                m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::TRAINING), TEXT("Layer_UI"), m_pUI_HP);
            }
        }
    }


    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CGomdol::Render()
{
    return S_OK;
}

void CGomdol::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::PLAYER_ATTACK == eLayer)
    {

        _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vHitDir = XMLoadFloat3(&vContactPoint) - vPosition;
        vHitDir = XMVector3Normalize(vHitDir);

        _vector vLook = XMVector3Normalize(
            m_pTransformCom->Get_State(STATE::LOOK));

        _float fDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));
        _float fCrossY = XMVectorGetY(XMVector3Cross(vLook, vHitDir));

        if (fabsf(fDot) >= fabsf(fCrossY))
        {
            // 앞 / 뒤
            if (fDot >= 0.f)
            {
                m_tHitDirInfo.Clear_Flag();
                m_tHitDirInfo.Add_Flag(m_tHitDirInfo.F);
            }
            else
            {
                m_tHitDirInfo.Clear_Flag();
                m_tHitDirInfo.Add_Flag(m_tHitDirInfo.B);

            }
        }
        else
        {
            // 좌 / 우
            if (fCrossY >= 0.f)
            {
                m_tHitDirInfo.Clear_Flag();
                m_tHitDirInfo.Add_Flag(m_tHitDirInfo.R);

            }
            else
            {
                m_tHitDirInfo.Clear_Flag();
                m_tHitDirInfo.Add_Flag(m_tHitDirInfo.L);

            }
        }



    }


    


}

void CGomdol::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CGomdol::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CGomdol::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.9f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 0.3f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 10.f;
    tCharVirDesc.fMaxStrength = 10.f;
    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepDownExtra = _float3(0.f, 0.f, 0.f);

    m_tGomdolCharvir.pGameObject = this;
    m_tGomdolCharvir.strName = TEXT("tGomdolChaVir");
    m_tGomdolCharvir.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.pCollisionDesc = &m_tGomdolCharvir;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    m_pCharVirCom->Collision_Active(true);



    return S_OK;
}

HRESULT CGomdol::Ready_PartObjects()
{
    CBody_Gomdol::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::TRAINING), TEXT("Prototype_PartObject_Gomdol_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Gomdol*>(pBody);
    Safe_AddRef(m_pBody);

    return S_OK;
}

HRESULT CGomdol::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;


#pragma region FrontAttack

    pModel->Register_Event("FrontAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pBody->Set_OnAttackCollision_RH(true);
        });

    pModel->Register_Event("FrontAttack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pBody->Set_OnAttackCollision_RH(false);
        });


#pragma endregion


#pragma region ComboAttack

    pModel->Register_Event("ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pBody->Set_OnAttackCollision_LH(true);
        });

    pModel->Register_Event("ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pBody->Set_OnAttackCollision_LH(false);
        });


    pModel->Register_Event("ComboAttack_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pBody->Set_OnAttackCollision_LH(true);
        });

    pModel->Register_Event("ComboAttack_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pBody->Set_OnAttackCollision_LH(false);
        });


#pragma endregion

#pragma region ROAR

#pragma endregion



    return S_OK;
}

HRESULT CGomdol::Ready_SFX()
{

    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

#pragma region WALK

    pModel->Register_Event("SFX_Walk_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_r_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Walk_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_l_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Walk_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_r_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Walk_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_l_04 (SFX).wav"), 0.6f);
        }
    );


#pragma endregion

#pragma region RUN

    pModel->Register_Event("SFX_Run_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_Footstep_Run_B_01 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Run_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_Footstep_Run_B_02 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Run_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_Footstep_Run_B_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Run_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_Footstep_Run_B_02 (SFX).wav"), 0.6f);
        }
    );

#pragma endregion

#pragma region

#pragma region FrontAttackSFX
    
    //vo
    pModel->Register_Event("SFXAASASWS_FrontAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFXAASASWS_FrontAttack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_03 (SFX).wav"), 0.6f);
        }
    );


#pragma endregion


#pragma region ComboAttackSFX

    pModel->Register_Event("SFX_VO_ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_VO_ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Atk_S_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_VO_ComboAttack_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Roar_S_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Roar_S_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Roar_S_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_03 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_ComboAttack_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_01 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_02 (SFX).wav"), 0.6f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_BigBear_1H_Swish_03 (SFX).wav"), 0.6f);
        }
    );


#pragma endregion


#pragma region SLEEP

    pModel->Register_Event("SFX_Sleep_Step_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_r_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Sleep_Step_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_l_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Sleep_Step_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_r_04 (SFX).wav"), 0.6f);

        }
    );

    pModel->Register_Event("SFX_Sleep_Step_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_bigbear_footstep_l_04 (SFX).wav"), 0.6f);
        }
    );

    pModel->Register_Event("SFX_Sleep_Sniff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_01 (SFX).wav"), 1.f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_02 (SFX).wav"), 1.f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_03 (SFX).wav"), 1.f);
        }
    );

    pModel->Register_Event("SFX_Sleep_Sniff", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()
        {
            _uint iSoundIndex = m_pGameInstance->Rand(0, 2);

            if (iSoundIndex == 0)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_01 (SFX).wav"), 1.f);
            else if (iSoundIndex == 1)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_02 (SFX).wav"), 1.f);
            else if (iSoundIndex == 2)
                m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_bigbear_idle_sniff_03 (SFX).wav"), 1.f);
        }
    );

#pragma endregion




    
    return S_OK;
}


void CGomdol::SFX_HIT(_uint iHitIndex)
{
    _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iHitIndex == 0)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_S_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_S_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_S_03 (SFX).wav"), 1.f);
    }

    else if (iHitIndex == 1)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_M_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_M_02 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_M_03 (SFX).wav"), 1.f);
    }

    else if (iHitIndex == 2)
    {
        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_L_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_L_01 (SFX).wav"), 1.f);
        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_BigBear_Dmg_L_01 (SFX).wav"), 1.f);
    }


}





CGomdol* CGomdol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGomdol* pInstance = new CGomdol(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CGomdol"));
    }
    return pInstance;
}

CGameObject* CGomdol::Clone(void* pArg)
{
    CGomdol* pInstance = new CGomdol(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CGomdol"));
    }
    return pInstance;
}

void CGomdol::Free()
{
    Safe_Release(m_pBody);
    __super::Free();
}
