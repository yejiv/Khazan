#include "Yetuga.h"
#include "GameInstance.h"
#include "AI_Controller_Yetuga.h"
#include "BlackBoard.h"

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

    m_pController = CAI_Controller_Yetuga::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

     m_pModelCom->Set_Animation(3);
     m_pModelCom->Set_AnimationLoop(true);

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(
            m_pGameInstance->Rand(-5.f, 5.f),
            2.f,
            m_pGameInstance->Rand(5.f, 5.f),
            1.f
        ));

    return S_OK;
}

void CYetuga::Priority_Update(_float fTimeDelta)
{
}

void CYetuga::Update(_float fTimeDelta)
{

 
    m_fCoolTimeAcc += fTimeDelta;

    m_pController->Update(this, fTimeDelta);


   /* if (true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;*/
}

void CYetuga::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CYetuga::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CYetuga::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
        
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

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
