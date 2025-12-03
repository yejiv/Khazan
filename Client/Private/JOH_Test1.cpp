#include "JOH_Test1.h"
#include "GameInstance.h"

CJOH_Test1::CJOH_Test1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CJOH_Test1::CJOH_Test1(const CJOH_Test1& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CJOH_Test1::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJOH_Test1::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_Animation(3);

   m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
   // m_pTransformCom->Scale(_float3(0.01f, 0.01f, 0.01f));
    //m_pTransformCom->Scale(_float3(10.f, 10.f, 10.f));

#ifdef _DEBUG
   //m_pGameInstance->AddWidget(TEXT("Client"), [this]() {
   //    ImGui::Begin("CJOH_Test1");
   //    m_pModelCom->Debug_RanderState();
   //    ImGui::End();
   //    });

#endif
    return S_OK;
}

void CJOH_Test1::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CJOH_Test1::Update(_float fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;

  /*  if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
    {
        m_pModelCom->Set_Animation(++m_iCurrentAnimIndex);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL,fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
    {
        m_iCurrentAnimIndex = 2;
        m_pModelCom->Set_Animation(m_iCurrentAnimIndex);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
    {
        m_pModelCom->Set_AnimationSet("Set1");
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_4))
    {
        m_pModelCom->Set_AnimationSet("Set2");
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_5))
    {
        m_pModelCom->Set_Animation(3);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_6))
    {
        m_iCurrentAnimIndex = 3;

        m_pModelCom->Set_Animation(m_iCurrentAnimIndex);
        m_pModelCom->Set_AnimationLoop(true);
    }*/
}

void CJOH_Test1::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

#ifdef _DEBUG

#endif
}

HRESULT CJOH_Test1::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        //if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
        //    return E_FAIL;
        ///*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)))
        //    return E_FAIL;        */
        //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }



    return S_OK;
}


HRESULT CJOH_Test1::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_JOH_TestModel"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
    //    return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CJOH_Test1::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CJOH_Test1* CJOH_Test1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJOH_Test1* pInstance = new CJOH_Test1(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CJOH_Test1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJOH_Test1::Clone(void* pArg)
{
    CJOH_Test1* pInstance = new CJOH_Test1(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CJOH_Test1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJOH_Test1::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
