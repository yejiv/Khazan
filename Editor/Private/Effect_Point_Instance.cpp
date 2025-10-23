#include "Effect_Point_Instance.h"
#include "Effect_Prefab.h"
#include "GameInstance.h"

CEffect_Point_Instance::CEffect_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect_Element{pDevice, pDeviceContext}
{
}

CEffect_Point_Instance::CEffect_Point_Instance(const CEffect_Point_Instance& Prototype)
    : CEffect_Element(Prototype)
{
}

HRESULT CEffect_Point_Instance::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    Apply(pArg);

    return S_OK;
}

void CEffect_Point_Instance::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEffect_Point_Instance::Update(_float fTimeDelta)
{
    for(auto it = m_TimeTracks.begin(); it != m_TimeTracks.end();)
    {
        it->fCurTime += fTimeDelta;

        if(it->fCurTime > it->fDurTime && it->EventType != 0)
        {
            dynamic_cast<CVIBuffer_Point_Instance*>(m_pVIBufferCom)->Remove_Speed(CVIBuffer_Point_Instance::SPEED_VALUE(it->EventType - 1));
            it = m_TimeTracks.erase(it);
        }
        else
            ++it;
    }

    m_bRunning = (m_pVIBufferCom->Update(fTimeDelta) == true && m_TimeTracks.size() == 0) ? false : true;

    if (m_sData.bGravity == true)
        m_pVIBufferCom->UpdateGravity(fTimeDelta);

    __super::Update(fTimeDelta);

    /* Edit */
    if (m_TimeTracks.size() == 0)
        m_pVIBufferCom->Remove_Speed();
}

void CEffect_Point_Instance::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Point_Instance::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    //m_pShaderCom->Begin((_uint)m_Data.TextureBindType);
    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Point_Instance::Save_Data(ofstream& os)
{
}

void CEffect_Point_Instance::Edit_Element()
{
    _int            isCircle = (_int)m_sEditingData.IsCircle;

    ImGui::RadioButton("Spawn_BoundingBox", &isCircle, 0);
    ImGui::RadioButton("Spawn_Circle", &isCircle, 1);

    if(isCircle == 0)
    {
        ImGui::InputFloat3("Center : ",reinterpret_cast<_float*>(&m_sEditingData.vCenter));
        ImGui::InputFloat3("Range : ",reinterpret_cast<_float*>(&m_sEditingData.vRange));
    }
    else 
        ImGui::InputFloat("Circle Offset  : ",&m_sEditingData.fOffset); 
    
    ImGui::InputScalar("Instance Num : ", ImGuiDataType_U32, &m_sEditingData.iNumInstance);

    ImGui::InputFloat2("Size : ", reinterpret_cast<_float*>(&m_sEditingData.vSize));
    ImGui::InputFloat("Size Ratio : ", &m_sEditingData.fSizeRatio);
    ImGui::InputFloat2("LifeTime : ", reinterpret_cast<_float*>(&m_sEditingData.vLifeTime));
    ImGui::Checkbox("Element Loop", &m_sEditingData.bIsLoop);

    ImGui::ColorEdit4("MyColorWithAlpha",(float*)&m_sEditingData.vColor);

    const char* textures[] = {"test0","test1","test2","test3","test4","test5"};
    ImGui::ListBox("Particles",reinterpret_cast<int*>(&m_sEditingData.iTextureIdx), textures,IM_ARRAYSIZE(textures));

    m_sEditingData.IsCircle = isCircle;

    if (ImGui::Button("Apply"))
        Apply(&m_sEditingData);
    if (ImGui::Button("Revert"))
        RevertChanges();
}

void CEffect_Point_Instance::RevertChanges()
{
    m_sEditingData = m_sData;
}

void CEffect_Point_Instance::Reset()
{
    __super::Reset();
    m_pVIBufferCom->Reset();
}

void CEffect_Point_Instance::SetSpreadData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::SPREAD_SPEED, data.fSpreadSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(data.eEventType), data.fDuration); 
}

void CEffect_Point_Instance::SetRotateData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::ROTATION_SPEED, data.fRotationSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetTwinkleData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::SCALE_SPEED, data.fScaleSpeed);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetUpwardData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Point_Instance::SPEED_VALUE::UPWARD_SPEED, data.fUpwardSpeed);
    SetData(ENUM_CLASS(data.eEventType),data.fDuration);
}

void CEffect_Point_Instance::SetScrollData(void* pArg)
{
    m_fScrollSpeed.x = *static_cast<_float*>(pArg);
    m_fScrollSpeed.y = *(static_cast<_float*>(pArg) + 1);
}

HRESULT CEffect_Point_Instance::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Particle_Prototype"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Point_Instance::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if(FAILED(m_pShaderCom->Bind_RawValue("g_vSourceColor", &m_sEditingData.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
        return E_FAIL;

    return S_OK;
}

void CEffect_Point_Instance::Apply(void* pArg)
{
    m_sData = *static_cast<PARTICLE_DESC*>(pArg);
    Safe_Release(m_pVIBufferCom);
    m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &m_sData);
    m_pVIBufferCom->Initialize_Clone(nullptr);
    m_iEffect_Type = 0;

    m_sEditingData = m_sData;
}

CEffect_Point_Instance* CEffect_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Point_Instance* pInstance = new CEffect_Point_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Point_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Point_Instance::Clone(void* pArg)
{
    CEffect_Point_Instance* pInstance = new CEffect_Point_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Point_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Point_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}





