#include "Yetuga.h"
#include "GameInstance.h"
#include "AI_Controller_Yetuga.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"
#include "CharacterVirtual.h"

CYetuga::CYetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice, pContext }
{
}

CYetuga::CYetuga(const CYetuga& Prototype)
    :CMonster{ Prototype }
{
}

HRESULT CYetuga::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CYetuga::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //-4 0 27
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-4, 0, 27, 1.f));


    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;





    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CYetuga::Update(_float fTimeDelta)
{
   /* _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    _float3 vTempDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>("Yetuga", "TargetDir");
    _vector vTargetDir = XMVector3Normalize(XMLoadFloat3(&vTempDir));
    _float fTurnSpeed = 1.f;
    _vector vLerpDir = XMVector3Normalize(XMVectorLerp(vLook, vTargetDir, fTimeDelta * fTurnSpeed));

    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + vLerpDir);*/

    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        _float3     vPickedPos{};
        _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
        if (true == isPicked)
        {
            m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f));
        }
    }

    m_pController->Update(this, fTimeDelta);

    __super::Update(fTimeDelta);

   

  
}

void CYetuga::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CYetuga::Render()
{
    return S_OK;
}

HRESULT CYetuga::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
 
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.5f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 1.;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CYetuga::Ready_PartObjects()
{
    CBody_Yetuga::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"),ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Yetuga*>(pBody);


    return S_OK;
}


CYetuga* CYetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CYetuga* pInstance = new CYetuga(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CYetuga"));
    }
    return pInstance;
}

CGameObject* CYetuga::Clone(void* pArg)
{
    CYetuga* pInstance = new CYetuga(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CYetuga"));
    }
    return pInstance;
}

void CYetuga::Free()
{
    __super::Free();
}
