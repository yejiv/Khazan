#include "Body_Khazan_Spear.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"

CBody_Khazan_Spear::CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CBody_Khazan_Spear::CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
{

}

_float4x4* CBody_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



HRESULT CBody_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_Spear::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SPEAR_DESC* pDesc = static_cast<BODY_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_iCurState = *m_pParentState;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvent()))
        return E_FAIL;


   // m_pModelCom->Set_Animation(5);
    /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

		ImGui::Begin("Sample Model State");

		m_pModelCom->Debug_RanderState();
		ImGui::End();
		});
#endif

    return S_OK;
}

void CBody_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CBody_Khazan_Spear::Update(_float fTimeDelta)
{
    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDelta);

    //m_pModelCom_Arm->Play_Animation(fTimeDelta);
    //m_pModelCom_Face->Play_Animation(fTimeDelta);
    //m_pModelCom_Hair->Play_Animation(fTimeDelta);
    //m_pModelCom_Leg->Play_Animation(fTimeDelta);
    //m_pModelCom_Shoes->Play_Animation(fTimeDelta);
    //m_pModelCom_Torso->Play_Animation(fTimeDelta);

    Update_CombinedMatrix();
}

void CBody_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        return;
    //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
    //    return;

#ifdef _DEBUG

#endif
}

HRESULT CBody_Khazan_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

      //  m_pShaderCom->Begin(1);

      //  m_pModelCom->Render(i);
    }

    Render_Part(m_pModelCom_Arm);
    Render_Part(m_pModelCom_Face);
    Render_Part(m_pModelCom_Hair);
    Render_Part(m_pModelCom_Leg);
    Render_Part(m_pModelCom_Shoes);
    Render_Part(m_pModelCom_Torso);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_CurrentShadowLightViewMatrix())))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_CurrentShadowLightProjMatrix())))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(2);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

void CBody_Khazan_Spear::Render_Part(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(1);
        pModel->Render(i);
    }

}

HRESULT CBody_Khazan_Spear::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Spear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
        TEXT("Com_Mode2"), reinterpret_cast<CComponent**>(&m_pModelCom_Arm), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
        TEXT("Com_Mode3"), reinterpret_cast<CComponent**>(&m_pModelCom_Face), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
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


    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_AnimationEvent()
{
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect1_Enter(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::EXIT,        [this]() {Effect1_Exit(); });
    m_pModelCom->Register_Event("Effect1", ANIM_EVENT_TRIGGERTYPE::CONTINUE,    [this]() {Effect1_Continue(); });
    m_pModelCom->Register_Event("Effect2", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect2(); });
    m_pModelCom->Register_Event("Effect3", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect3(); });
    m_pModelCom->Register_Event("Effect4", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect4(); });
    m_pModelCom->Register_Event("Effect5", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect5(); });
    m_pModelCom->Register_Event("Effect6", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect6(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::ENTER,       [this]() {Effect7_Enter(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::EXIT,        [this]() {Effect7_Exit(); });
    m_pModelCom->Register_Event("Effect7", ANIM_EVENT_TRIGGERTYPE::CONTINUE,    [this]() {Effect7_Continue(); });

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


	return S_OK;
}

void CBody_Khazan_Spear::Effect1_Enter()
{
    char msg[256];
    sprintf_s(msg,"Local Bone SpearFX\n %.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n", 
        m_pSpearFX_Matrix->_11, m_pSpearFX_Matrix->_12, m_pSpearFX_Matrix->_13, m_pSpearFX_Matrix->_14,
        m_pSpearFX_Matrix->_21, m_pSpearFX_Matrix->_22, m_pSpearFX_Matrix->_23, m_pSpearFX_Matrix->_24,
        m_pSpearFX_Matrix->_31, m_pSpearFX_Matrix->_32, m_pSpearFX_Matrix->_33, m_pSpearFX_Matrix->_34,
        m_pSpearFX_Matrix->_41, m_pSpearFX_Matrix->_42, m_pSpearFX_Matrix->_43, m_pSpearFX_Matrix->_44);
    OutputDebugStringA(msg);

    _float4x4 matWorldSpearFX;
    XMStoreFloat4x4(&matWorldSpearFX, m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearFX_Matrix) * m_pParentTransform->Get_WorldMatrix());
    sprintf_s(msg, "Local Bone SpearFX\n %.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n%.3f  %.3f  %.3f  %.3f\n",
        matWorldSpearFX._11, matWorldSpearFX._12, matWorldSpearFX._13, matWorldSpearFX._14,
        matWorldSpearFX._21, matWorldSpearFX._22, matWorldSpearFX._23, matWorldSpearFX._24,
        matWorldSpearFX._31, matWorldSpearFX._32, matWorldSpearFX._33, matWorldSpearFX._34,
        matWorldSpearFX._41, matWorldSpearFX._42, matWorldSpearFX._43, matWorldSpearFX._44);
    OutputDebugStringA(msg);
}

void CBody_Khazan_Spear::Effect1_Exit()
{
    //cout << "[Effect1_Exit]" << endl;
   // OutputDebugStringA("[Effect1_Exit] \n");

}

void CBody_Khazan_Spear::Effect1_Continue()
{
    //cout << "[Effect1_Continue]" << endl;
    //OutputDebugStringA("[Effect1_Continue] \n");

}

void CBody_Khazan_Spear::Effect2()
{
    /* OutputDebugStringA 원래 출력이 느립니다! 정확한 시점의 디버그는 cout으로 하십시오! */
    OutputDebugStringA("[Effect2] \n");

}

void CBody_Khazan_Spear::Effect3()
{
    OutputDebugStringA("[Effect3] \n");

}

void CBody_Khazan_Spear::Effect4()
{
    OutputDebugStringA("[Effect4] \n");

}

void CBody_Khazan_Spear::Effect5()
{
    OutputDebugStringA("[Effect5] \n");

}

void CBody_Khazan_Spear::Effect6()
{
    OutputDebugStringA("[Effect6] \n");

}

void CBody_Khazan_Spear::Effect7_Enter()
{
    OutputDebugStringA("[Effect7_Enter] \n");

}

void CBody_Khazan_Spear::Effect7_Exit()
{
    OutputDebugStringA("[Effect7_Exit] \n");

}

void CBody_Khazan_Spear::Effect7_Continue()
{
    OutputDebugStringA("[Effect7_Continue] \n");

}



inline _bool CBody_Khazan_Spear::Has_States()
{

	for (_uint i = 0; i < GetBitPosition(CKhazan_Sample::END); ++i)
	{
        if (Has_State(1 << i))
            return true;

	}
	return false;
}

CBody_Khazan_Spear* CBody_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_Spear::Clone(void* pArg)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Khazan_Spear::Free()
{
    __super::Free();

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom_Torso);
	Safe_Release(m_pModelCom_Arm);
	Safe_Release(m_pModelCom_Face);
	Safe_Release(m_pModelCom_Hair);
	Safe_Release(m_pModelCom_Leg);
	Safe_Release(m_pModelCom_Shoes);
}
