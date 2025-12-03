#include "CloudSphere.h"
#include "GameInstance.h"

CCloudSphere::CCloudSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{

}

CCloudSphere::CCloudSphere(const CCloudSphere& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CCloudSphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCloudSphere::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    CLOUD_SPHERE_DESC* pDesc = static_cast<CLOUD_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_CloudDesc = pDesc->CloudDesc;

    m_pTransformCom->Scale(_float3(0.0001f, 0.0001f, 0.0001f));

#ifdef _DEBUG
    m_FixDesc = m_CloudDesc;

    LEVEL eLevel = CClientInstance::GetInstance()->Get_CurrLevel();
    if (LEVEL::LOADING == eLevel || LEVEL::TITLE == eLevel)
        return S_OK;

    //Debug_CloudEdit();
#endif // _DEBUG

    return S_OK;
}

void CCloudSphere::Priority_Update(_float fTimeDelta)
{
}

void CCloudSphere::Update(_float fTimeDelta)
{
    _float4 vOffsetPos = *m_pGameInstance->Get_CamPosition();

    if (LEVEL::VIPER != CClientInstance::GetInstance()->Get_CurrLevel())
        vOffsetPos.y -= 4.f;
    else
        vOffsetPos.y -= 6.f;

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vOffsetPos));

    m_fTimeAcc += fTimeDelta;

#ifdef _DEBUG
    //if (m_pGameInstance->Key_Pressing(DIK_NUMPAD7, 0.f) && m_pGameInstance->Key_Pressing(DIK_NUMPAD8, 0.f) && m_pGameInstance->Key_Down(DIK_NUMPAD9))
    //    m_isCloudWindow = !m_isCloudWindow;
#endif // _DEBUG

    if (false == m_isTransition)
        return;

    // 스카이 박스 보간
    m_fTransTimeAcc += fTimeDelta;

    _float fRatio = m_fTransTimeAcc / m_fDuration;
    if (1.f <= fRatio)
    {
        fRatio = 1.f;
        m_isTransition = false;
    }

    m_CloudDesc.vCloudColor = Lerp(m_StartCloudDesc.vCloudColor, m_LerpCloudDesc.vCloudColor, fRatio);
    m_CloudDesc.fCloudScale = Lerp(m_StartCloudDesc.fCloudScale, m_LerpCloudDesc.fCloudScale, fRatio);
    m_CloudDesc.fCloudDensity = Lerp(m_StartCloudDesc.fCloudDensity, m_LerpCloudDesc.fCloudDensity, fRatio);
    m_CloudDesc.fCloudLightIntensity = Lerp(m_StartCloudDesc.fCloudLightIntensity, m_LerpCloudDesc.fCloudLightIntensity, fRatio);

    XMFLOAT3 vDir = Lerp(m_StartCloudDesc.vLightDir, m_LerpCloudDesc.vLightDir, fRatio);
    XMStoreFloat3(&m_CloudDesc.vLightDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
}

void CCloudSphere::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this)))
        return;
}

HRESULT CCloudSphere::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(Bind_Cloud_ShaderResources(), E_FAIL);

        m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float));

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(2), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CCloudSphere::Start_LerpCloud(CLOUD_DESC LerpCloudDesc, _float fDuration)
{
    m_isTransition = true;

    m_fTransTimeAcc = 0.f;
    m_fDuration = fDuration;

    m_StartCloudDesc = m_CloudDesc;
    m_LerpCloudDesc = LerpCloudDesc;

    m_CloudDesc.fDynamic = LerpCloudDesc.fDynamic;
    m_CloudDesc.fCloudSpeed = LerpCloudDesc.fCloudSpeed;
}

HRESULT CCloudSphere::Ready_Components(void* pArg)
{
    CLOUD_SPHERE_DESC* pDesc = static_cast<CLOUD_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxSphere"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CloudMesh"),
        TEXT("Com_Model_Cloud"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

#pragma region 구 클라우드 관련 텍스쳐들
    /* Prototype_Component_Texture_Cloud_Dist_Gradation */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Dist_Gradation"),
        TEXT("Com_Texture_Gradation"), reinterpret_cast<CComponent**>(&m_pTextureCom[DISTANCE_GRADATION]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_LookUp */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
        TEXT("Com_Texture_LookUp"), reinterpret_cast<CComponent**>(&m_pTextureCom[LOOKUP]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_Normal */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Normal"),
        TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[NORMAL]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_Distortion */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Distortion"),
        TEXT("Com_Texture_Distortion"), reinterpret_cast<CComponent**>(&m_pTextureCom[DISTORTION]), nullptr)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CCloudSphere::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCloudSphere::Bind_Cloud_ShaderResources()
{
    m_pTextureCom[DISTANCE_GRADATION]->Bind_Shader_Resource(m_pShaderCom, "g_GradationTexture", 0);
    m_pTextureCom[LOOKUP]->Bind_Shader_Resource(m_pShaderCom, "g_LookUpTexture", 0);
    m_pTextureCom[NORMAL]->Bind_Shader_Resource(m_pShaderCom, "g_NormalTexture", 0);
    m_pTextureCom[DISTORTION]->Bind_Shader_Resource(m_pShaderCom, "g_DistortionTexture", 0);

    m_pShaderCom->Bind_RawValue("g_fCloudDensity", &m_CloudDesc.fCloudDensity, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fCloudLightIntensity", &m_CloudDesc.fCloudLightIntensity, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fCloudScale", &m_CloudDesc.fCloudScale, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fCloudSpeed", &m_CloudDesc.fCloudSpeed, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_isDynamic", &m_CloudDesc.fDynamic, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_vCloudColor", &m_CloudDesc.vCloudColor, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_vLightDir", &m_CloudDesc.vLightDir, sizeof(_float3));

    return S_OK;
}
#ifdef _DEBUG
void CCloudSphere::Debug_CloudEdit()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {
        if (m_isCloudWindow)
        {
            // 메인 윈도우
            ImGui::Begin("Cloud Sphere Edit", &m_isCloudWindow, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("CLOUD COLOR");
            ImGui::Text("COLOR PALHETT");
            ImGui::ColorPicker3("##r_edit", reinterpret_cast<_float*>(&m_CloudDesc.vCloudColor));
            ImGui::Separator();

            ImGui::Text("CLOUD SCALE"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloud_scale", &m_CloudDesc.fCloudScale, 0.01f, 0.1f);
            ImGui::Separator();

            ImGui::Text("CLOUD SPEED"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloud_speed", &m_CloudDesc.fCloudSpeed, 0.01f, 0.1f);
            ImGui::Separator();

            ImGui::Text("CLOUD DENSITY"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloud_density", &m_CloudDesc.fCloudDensity, 0.01f, 0.1f);
            ImGui::Separator();

            ImGui::Text("CLOUD INTENSITY"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloud_inten", &m_CloudDesc.fCloudLightIntensity, 0.01f, 0.1f);
            ImGui::Separator();

            ImGui::Text("LIGHT DIRECTION");
            ImGui::Text("X"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloudlight_dir_X", &m_CloudDesc.vLightDir.x, 0.01f, 0.1f);
            ImGui::Text("Y"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloudlight_dir_Y", &m_CloudDesc.vLightDir.y, 0.01f, 0.1f);
            ImGui::Text("Z"); ImGui::SameLine();
            ImGui::InputFloat("##fix_cloudlight_dir_Z", &m_CloudDesc.vLightDir.z, 0.01f, 0.1f);
            ImGui::Separator();

            ImGui::Text("CLOUD DYNAMIC"); ImGui::SameLine();
            if (ImGui::Button("ON"))
                m_CloudDesc.fDynamic = 1.f;
            ImGui::SameLine();
            if (ImGui::Button("OFF"))
                m_CloudDesc.fDynamic = 0.f;

            ImGui::Separator();
            ImGui::Text("DEFAULT FILE PATH : %s", m_szFilePath);
            if (ImGui::Button("HEINMACH")) m_eMapType = KHAZAN_MAP::HEINMACH; ImGui::SameLine();
            if (ImGui::Button("EMBARS")) m_eMapType = KHAZAN_MAP::EMBARS; ImGui::SameLine();
            if (ImGui::Button("VIPER")) m_eMapType = KHAZAN_MAP::VIPER;

            switch (m_eMapType)
            {
            case KHAZAN_MAP::HEINMACH:
                m_strFolderName = "HeinMach/";
                ImGui::Text("CURRENT : HEINMACH FOLDER");
                break;
            case KHAZAN_MAP::EMBARS:
                m_strFolderName = "Embars/";
                ImGui::Text("CURRENT : EMBARS FOLDER");
                break;
            case KHAZAN_MAP::VIPER:
                m_strFolderName = "Viper/";
                ImGui::Text("CURRENT : VIPER FOLDER");
                break;
            default:
                break;
            }

            ImGui::Text("SAVE FILE NAME : "); ImGui::SameLine();
            ImGui::InputText("##cloud_file_name", m_szFileName, IM_ARRAYSIZE(m_szFileName));

            ImGui::Separator();
            if (ImGui::Button("RESET")) m_CloudDesc = m_FixDesc;
            if (ImGui::Button("REFRESH ORIGIN DATA")) m_FixDesc = m_CloudDesc;

            ImGui::Separator();
            if (ImGui::Button("SAVE"))
            {
                string strPath = m_szFilePath;
                strPath += m_strFolderName;
                strPath += m_szFileName;
                strPath += "_cloud.dat";

                DWORD dwByte = {};

                HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (INVALID_HANDLE_VALUE == hFile)
                {
                    _int a = 10;
                }
                else
                {
                    WriteFile(hFile, &m_CloudDesc, sizeof(CLOUD_DESC), &dwByte, nullptr);
                }

                CloseHandle(hFile);

            } ImGui::SameLine();
            if (ImGui::Button("LOAD"))
            {
                string strPath = m_szFilePath;
                strPath += m_strFolderName;
                strPath += m_szFileName;
                strPath += "_cloud.dat";

                DWORD dwByte = {};

                HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (INVALID_HANDLE_VALUE == hFile)
                {
                    _int a = 10;
                }
                else
                {
                    ReadFile(hFile, &m_CloudDesc, sizeof(CLOUD_DESC), &dwByte, nullptr);
                }

                CloseHandle(hFile);
            }

            ImGui::End();
        }
        });
}
#endif
CCloudSphere* CCloudSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCloudSphere* pInstance = new CCloudSphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCloudSphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCloudSphere::Clone(void* pArg)
{
    CCloudSphere* pInstance = new CCloudSphere(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCloudSphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCloudSphere::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);

    Safe_Release(m_pModelCom);

    for (auto& pTex : m_pTextureCom)
        Safe_Release(pTex);
}
