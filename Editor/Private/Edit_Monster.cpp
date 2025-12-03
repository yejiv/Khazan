#include "Edit_Monster.h"
#include "AIController_Edit.h"

CEdit_Monster::CEdit_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CContainerObject{pDevice,pContext}
{
}

CEdit_Monster::CEdit_Monster(const CEdit_Monster& Prototype)
    :CContainerObject{Prototype}
{
}

HRESULT CEdit_Monster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEdit_Monster::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

   /* m_pController = CAIController_Edit::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;*/

    return S_OK;
}

void CEdit_Monster::Priority_Update(_float fTimeDelta)
{
}

void CEdit_Monster::Update(_float fTimeDelta)
{
    //m_pController->Update(this,fTimeDelta);
}

void CEdit_Monster::Late_Update(_float fTimeDelta)
{
}

HRESULT CEdit_Monster::Render()
{
    return S_OK;
}

HRESULT CEdit_Monster::Ready_Components()
{
    return S_OK;
}

CEdit_Monster* CEdit_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEdit_Monster* pInstance = new CEdit_Monster(pDevice,pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CEdit_Monster"));
    }
    return pInstance;
}

CGameObject* CEdit_Monster::Clone(void* pArg)
{
    CEdit_Monster* pInstance = new CEdit_Monster(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CEdit_Monster"));
    }
    return pInstance;
}

void CEdit_Monster::Free()
{
    __super::Free();

    //Safe_Release(m_pController);
}
