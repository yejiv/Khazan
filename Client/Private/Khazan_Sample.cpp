#include "Khazan_Sample.h"
#include "Body_Khazan_Sample.h"
#include "Spear_Khazan_Sample.h"
#include "GameInstance.h"

#include "RigidBody.h"
#include "CharacterVirtual.h"

#include "Damage_Text.h"

CKhazan_Sample::CKhazan_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature{ pDevice, pContext }
{
}

CKhazan_Sample::CKhazan_Sample(const CKhazan_Sample& Prototype)
	: CCreature{ Prototype }
{
}

HRESULT CKhazan_Sample::Initialize_Prototype()
{
	return S_OK;

}

HRESULT CKhazan_Sample::Initialize_Clone(void* pArg)
{
	CREATURE_DESC desc{};

	desc.fAttack = 10.f;
	desc.fMaxHP = 100.f;
    desc.fMaxStamina = 100.f;
	desc.fMoveSpeed = 10.f;
	desc.fRotationPerSec = XMConvertToRadians(180.f);
	desc.fSpeedPerSec = 1.f;

    m_fCurrentHP = 100.f;
    m_fCurrentStamina = 100.f;
    if (FAILED(__super::Initialize_Clone(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Collision()))
        return E_FAIL;

#ifdef _DEBUG
    Debug_Widget();
#endif // _DEBUG

    return S_OK;

}

void CKhazan_Sample::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CKhazan_Sample::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_R))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::DEFAULT, m_pTransformCom->Get_State(STATE::POSITION), 100, { 0.f, 10.f });
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }

    if (m_pGameInstance->Key_Down(DIK_T))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::BACK, m_pTransformCom->Get_State(STATE::POSITION), 1234);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }
    if (m_pGameInstance->Key_Down(DIK_Y))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::SPECIAL, m_pTransformCom->Get_State(STATE::POSITION), 12345657656);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }
    if (m_pGameInstance->Key_Down(DIK_U))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::PLAYER, m_pTransformCom->Get_State(STATE::POSITION), 100);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }

    if (m_isEnableControl)
    {
        if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
        {
            _float3     vPickedPos{};
            _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
            if (true == isPicked)
            {
                m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f));
                m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 1.f));
            }
        }

        Update_State(fTimeDelta);
    }
    __super::Update(fTimeDelta);

    XMStoreFloat4x4(&m_SpearFX_WorldMatrix, m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearFX_Matrix) * m_pTransformCom->Get_WorldMatrix());

    //m_pRigidBodyCom->Update(fTimeDelta, m_pTransformCom->Get_WorldMatrix());

    m_pCharVirCom->Sync_Update(m_pTransformCom);
    m_pCharVirCom->Update(fTimeDelta, m_pTransformCom);
}

void CKhazan_Sample::Late_Update(_float fTimeDelta)
{
    



    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CKhazan_Sample::Render()
{


	return S_OK;

}

void CKhazan_Sample::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CKhazan_Sample::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}


void CKhazan_Sample::Update_State(_float fTimeDelta)
{
    Key_Input(fTimeDelta);

}

void CKhazan_Sample::Key_Input(_float fTimeDelta)
{

    if (!Has_State(ATTACK_ALL))
    {
        if (m_pGameInstance->Key_Down(DIK_S))
        {
            ++m_isMove;
            Add_DirState(DOWN);
        }
        if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
        {
            m_pTransformCom->Go_Backward(fTimeDelta * m_fMoveSpeed);
        }
        if (m_pGameInstance->Key_Up(DIK_S))
        {
            m_isMove = 0;
           // m_isMove = m_isMove - 1 < 0 ? 0 : m_isMove - 1;
            Remove_DirState(DOWN);
        }

        if (m_pGameInstance->Key_Down(DIK_W))
        {
            ++m_isMove;
            Add_DirState(UP);
        }
        if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
        {
            m_pTransformCom->Go_Straight(fTimeDelta * m_fMoveSpeed);
        }
        if (m_pGameInstance->Key_Up(DIK_W))
        {
            m_isMove = 0;

            //m_isMove = m_isMove - 1 < 0 ? 0 : m_isMove - 1;
            Remove_DirState(UP);
        }

        if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);

        if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
    }

    if (m_isMove > 0)
    {
        Add_State(WALK);
        Remove_State(IDLE);
    }
    else
    { 
        Add_State(IDLE);
        Remove_State(MOVING);
    }


    if (Has_State(WALK) && m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        Add_State(RUN);
    }
    else if (Has_State(WALK) && m_pGameInstance->Key_Up(DIK_LSHIFT))
    {
        Remove_State(RUN);
    }


	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		Clear_State();
        m_isMove = 0; 

		Add_State(ATTACK_FAST);
	}

	if (m_pGameInstance->Key_Down(DIK_X))
	{
		Clear_State();
        m_isMove = 0;
		Add_State(ATTACK_SET);
	}

}

HRESULT CKhazan_Sample::Ready_Components()
{
    return S_OK;
}

HRESULT CKhazan_Sample::Ready_PartObjects()
{
    CBody_Khazan_Sample::BODY_KHAZAN_SAMPLE_DESC         BodyDesc{};
    BodyDesc.pState = &m_iState;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Body_Khazan_Sample"), &BodyDesc)))
        return E_FAIL;

    pBody = static_cast<CBody_Khazan_Sample*>(Find_PartObject(TEXT("Part_Body")));
    m_pWeaponR_Matrix = pBody->Get_BoneMatrix("Weapon_R");

    CSpear_Khazan_Sample::SPEAR_KHAZAN_SAMPLE_DESC         SpearDesc{};
    SpearDesc.pState = &m_iState;
    SpearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SpearDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon_Spear"), ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Spear_Khazan_Sample"), &SpearDesc)))
        return E_FAIL;

    pSpear = static_cast<CSpear_Khazan_Sample*>(Find_PartObject(TEXT("Part_Weapon_Spear")));
    m_pSpearFX_Matrix = pSpear->Get_BoneMatrix("FX");
    m_SpearOffset_Matrix = pSpear->Get_OffestMatrix();

    /* 넘겨주기  */
    pSpear->Set_matWeaponR(m_pWeaponR_Matrix);
    pBody->Set_matSpearFX(m_pSpearFX_Matrix);
    pBody->Set_matSpearOffset(m_SpearOffset_Matrix);

	return S_OK;

}

HRESULT CKhazan_Sample::Ready_Collision()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.7f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 0.5f;
    tCharVirDesc.fHeight = 0.5f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}
#ifdef _DEBUG
inline _bool CKhazan_Sample::Has_States()
{
    for (_uint i = 0; i < GetBitPosition(CKhazan_Sample::END); ++i)
    {
        if (Has_State(1 << i))
            return true;

    }
    return false;
}
void CKhazan_Sample::Debug_Widget()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

        ImGui::Begin("Control Guide");

        //  Movement
        ImGui::BeginChild("LeftPanel", ImVec2(220, 0), true);
        {
            ImGui::Text("Movement");
            ImGui::Separator();
            ImGui::BulletText("U key Move Forward (WALK)");
            ImGui::BulletText("D key Move Backward");
            ImGui::BulletText("L key Turn Left");
            ImGui::BulletText("R key Turn Right");
            ImGui::BulletText("U key + LSHIFT  Run (RUN)");
        }
        ImGui::EndChild();

        // Attack & Other
        ImGui::SameLine();
        ImGui::BeginChild("MiddlePanel", ImVec2(300, 0), true);
        {
            ImGui::Text("Attack");
            ImGui::Separator();
            ImGui::BulletText("Z  Fast Attack (ATTACK_FAST)");
            ImGui::BulletText("X  Set Attack (ATTACK_SET)");

            ImGui::Spacing();
            ImGui::Text("Other");
            ImGui::Separator();
            ImGui::BulletText("LShift + Mouse(LB) Ground click -> teleport");
            ImGui::BulletText("Idle state when not moving");
        }
        ImGui::EndChild();

        // 오른쪽 패널: 사용자 정의 공간
        ImGui::SameLine();
        ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
        {
			if(ImGui::Button(m_isEnableControl ? "Clicked Enable Control" : "Clicked Disable Control", ImVec2(-1.0f, 0.0f)))
				m_isEnableControl = !m_isEnableControl;

            ImGui::Text("Maunal Area");
            // === Speed Section ===
            ImGui::Separator();
			ImGui::DragFloat("Move Speed", &m_fMoveSpeed, 0.1f, 0.f, 100.f);

            // === HP Section ===
            ImGui::Text("Health");
            ImGui::Separator();
            ImGui::SliderFloat("Current HP", &m_fCurrentHP, 0.0f, m_fMaxHP, "%.1f"); 
            ImGui::InputFloat("Max HP", &m_fMaxHP);
            ImGui::ProgressBar(m_fCurrentHP / max(0.0001f, m_fMaxHP), ImVec2(-1.0f, 0.0f), "HP");

            // === Stamina Section ===
            ImGui::Spacing();
            ImGui::Text("Stamina");
            ImGui::Separator();
            ImGui::SliderFloat("Current Stamina", &m_fCurrentStamina, 0.0f, m_fMaxStamina, "%.1f");
            ImGui::InputFloat("Max Stamina", &m_fMaxStamina);
            ImGui::ProgressBar(m_fCurrentStamina / max(0.0001f, m_fMaxStamina), ImVec2(-1.0f, 0.0f), "Stamina");

            // === Attack Section ===
            ImGui::Spacing();
            ImGui::Text("Attack");
            ImGui::Separator();
            ImGui::SliderFloat("Attack Power", &m_fAttack, 0.0f, 500.0f, "%.1f");
        }
        ImGui::EndChild();

        ImGui::End();


		});
}
#endif // _DEBUG

CKhazan_Sample* CKhazan_Sample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKhazan_Sample* pInstance = new CKhazan_Sample(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKhazan_Sample::Clone(void* pArg)
{
    CKhazan_Sample* pInstance = new CKhazan_Sample(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CKhazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_Sample::Free()
{
    __super::Free();
    //Safe_Release(m_pRigidBodyCom);
    Safe_Release(m_pCharVirCom);
}
