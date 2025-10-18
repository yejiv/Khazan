#include "Prop_Export.h"

#include "GameInstance.h"

#include "Editor_Model.h"

CProp_Export::CProp_Export(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Export::CProp_Export(const CProp_Export& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Export::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Export::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    return S_OK;
}

void CProp_Export::Priority_Update(_float fTimeDelta)
{
}

void CProp_Export::Update(_float fTimeDelta)
{
    _wstring strModelName = { m_szModelName };

    m_strExportPath += WStringToAnsi(strModelName);

    // Material 경로 오현이형한테 물어보기
    m_pModelCom->ExportModel(m_strExportPath);
    m_isDead = true;
}

void CProp_Export::Late_Update(_float fTimeDelta)
{
    //m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CProp_Export::Render()
{
    return S_OK;
}

HRESULT CProp_Export::Ready_Components(void* pArg)
{
    PROP_EXPORT_DESC* pDesc = static_cast<PROP_EXPORT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LEVEL 미지정"), E_FAIL);

    memcpy(m_szModelName, pDesc->szModelName, MAX_PATH);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CProp_Export* CProp_Export::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Export* pInstance = new CProp_Export(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Export"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Export::Clone(void* pArg)
{
    CProp_Export* pInstance = new CProp_Export(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Export"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Export::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
