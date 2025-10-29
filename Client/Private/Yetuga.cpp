#include "Yetuga.h"
#include "GameInstance.h"
#include "AI_Controller_Yetuga.h"
#include "BlackBoard.h"
#include "Body_Yetuga.h"

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

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    /* m_pModelCom->Set_Animation(3);
     m_pModelCom->Set_AnimationLoop(true);*/

     //-4 0 27
     m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-4, 0, 27, 1.f));




    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CYetuga::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    CContainerObject::Update(fTimeDelta);

  
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

    return S_OK;
}

HRESULT CYetuga::Ready_PartObjects()
{
    CBody_Yetuga::PARTOBJECT_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"),ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_PartObject_Yetuga_Body"), &BodyDesc)))
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
