#include "Trigger.h"

#include "GameInstance.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CTrigger::CTrigger(const CTrigger& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CTrigger::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CTrigger::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_strTriggerKey = pDesc->strTriggerKey;

    return S_OK;
}

void CTrigger::Priority_Update(_float fTimeDelta)
{
}

void CTrigger::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_L))
        m_isRender = !m_isRender;
}

void CTrigger::Late_Update(_float fTimeDelta)
{
    if (true == m_isRender)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CTrigger::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(1), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CTrigger::Ready_Components(void* pArg)
{
    TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_Trigger"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CTrigger::Ready_Collision(void* pArg)
{
    CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
    TriggerDesc.vExtent = _float3(3.f, 3.f, 3.f);
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vPos = vPos;
    TriggerDesc.vQuat = vQuat;
    TriggerDesc.vShapeOffset = _float3(0.f, 0.0f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;

    return S_OK;
}

void CTrigger::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CTrigger::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CTrigger::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrigger* pInstance = new CTrigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTrigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
    CTrigger* pInstance = new CTrigger(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTrigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrigger::Free()
{
    __super::Free();

    Safe_Release(m_pTriggerCom);
}
