#include "Effect_Mesh_Instance.h"
#include "Effect_Prefab.h"
#include "GameInstance.h"

CEffect_Mesh_Instance::CEffect_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect_Element{pDevice, pDeviceContext}
{
}

CEffect_Mesh_Instance::CEffect_Mesh_Instance(const CEffect_Mesh_Instance& Prototype)
    : CEffect_Element(Prototype)
{
}

HRESULT CEffect_Mesh_Instance::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    Apply(pArg);

    return S_OK;
}

void CEffect_Mesh_Instance::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEffect_Mesh_Instance::Update(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta;

    for(auto it = m_TimeTracks.begin(); it != m_TimeTracks.end();)
    {
        it->fCurTime += fTimeDelta;

        //if(it->fCurTime > it->fDurTime && it->EventType != 1)
        if(it->fCurTime > it->fDurTime)
        {
            if (m_pVIBufferCom->isLoop() && m_TimeTracks.size() == 1)
            {
                ++it;
                continue;
            }
            dynamic_cast<CVIBuffer_Mesh_Instance*>(m_pVIBufferCom)->Remove_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE(it->EventType));
            it = m_TimeTracks.erase(it);
        }
        else
            ++it;
    }

    m_bRunning = (m_pVIBufferCom->Update(fTimeDelta) == true && m_TimeTracks.size() == 0) ? false : true;
    
    if (m_sData.bGravity)
        m_pVIBufferCom->UpdateGravity(fTimeDelta);

    if (m_sData.bIsTurbulence)
        m_pVIBufferCom->UpdateTurbulence(fTimeDelta, m_fAccTime);

    __super::Update(fTimeDelta);

    /* Edit */
    //if (m_TimeTracks.size() == 0)
    if (!m_bRunning)
        m_pVIBufferCom->Remove_Speed(); 
}

void CEffect_Mesh_Instance::Late_Update(_float fTimeDelta)
{
    //__super::Late_Update(fTimeDelta);
    if (m_bIsNormal)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
    else
        __super::Late_Update(fTimeDelta);

}

HRESULT CEffect_Mesh_Instance::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (m_bIsNormal)
        m_pShaderCom->Begin(2);
    else
        m_pShaderCom->Begin((_uint)m_sData.bIsFresnel);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Mesh_Instance::Save_Data(ofstream& os)
{
    os.write(reinterpret_cast<char*>(&m_iEffect_Type), sizeof(_uint));
    os.write(reinterpret_cast<char*>(&m_sData), sizeof(PARTICLE_DESC));
}

void CEffect_Mesh_Instance::Edit_Element()
{
    _int            isCircle = (_int)m_sEditingData.IsCircle;
    _bool            loop = (_int)m_sEditingData.bIsLoop;
    _bool            IsVerticalScroll = (_int)m_sEditingData.bIsScrollVertical;
    _bool            IsInverseScroll = (_int)m_sEditingData.bIsScrollInverse;
    _bool            IsFresnel = (_int)m_sEditingData.bIsFresnel;
    _bool            bIsTurbulence = (_int)m_sEditingData.bIsTurbulence;
    _bool            bIsDissolve = (_int)m_sEditingData.sDissolveData.bIsDissolve;

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
    ImGui::InputFloat3("Rotation : ", reinterpret_cast<_float*>(&m_sEditingData.fRotation));
    ImGui::InputFloat2("LifeTime : ", reinterpret_cast<_float*>(&m_sEditingData.vLifeTime));
    ImGui::InputFloat2("Scrolling Speed : ", reinterpret_cast<_float*>(&m_sEditingData.iScrollSpeed));
    ImGui::Checkbox("Element Loop", &loop);

    ImGui::ColorEdit4("MyColorWithAlpha",(float*)&m_sEditingData.vColor);

    const char* textures[] = { "test0", "test1", "test2",  "test3",  "test4",  "test5",  "test6" ,  "test7" ,  "test8" ,  "test9" ,  "test10" ,  "test11" ,  "test12",  "test13",  "test14",  "test15",  "test16",  "test17",  "test18",  "test19",  "test20",
        "shock", "smoke", "cloud", "blood", "Ice1", "Ice2", "Rock", "Slash28", "Slash29" , "Slash30" , "Slash31" , "Slash32", "Slash33", "Viper_Slash0", "Viper_Slash1", "Viper_Slash2", "Viper_Slash3", "Viper_Slash4" };

    ImGui::Combo("Mesh Textures", reinterpret_cast<int*>(&m_sEditingData.iTextureIdx), textures, IM_ARRAYSIZE(textures));


    const char* Meshes[] = { "Helix0", "Helix1", "Helix2", "Helix3",  "Helix4",  "Helix5",  "Helix6",  "Helix7",  "Helix8",  "Helix9",  "Helix10",  "Helix11",  "Helix12",  "Helix13",  "Helix14",  "Helix15",  "Helix16",  "Helix17",  "Helix18",  "Helix19",  "Helix20",
                                    "Spline0",  "Spline1", "Spline2",  "Spline3", "Spline4",  "Spline5",
                                    "Spline_Cylinder",  "Spline_Helix0", "Spline_Helix1",
                                    "Spline_Twist0", "Spline_Twist1",  "Spline_Twist2", "Spline_Twist3",  "Spline_Twist4", "Spline_Twist5",  "Spline_Twist6", "Spline_Twist7",
                                    "TombStone_Spline0", "TombStone_Spline1", "TombStone_Spline2", "TombStone_Spline3", "WorldSpline",
                                    "Helix21", "absolb_Spiral", "Focus", "HelixSimple", "Lightning0", "Lightning1", "Lightning2" , "Lightning3", "Helix22", "Helix23", "Helix24", "Helix25", "Helix26",
                                    "Trail", "TwistRing0", "TwistRing1", "Spiral", "Wave0", "Wave1", "windLine", "Mash_Helix_Simple_mid", "T_blunt_slash_test", "Mash_Helix", "Mash_circle_twist",
                                    "Attack_ShockWave", "AttackTrail_First", "AttackTrail_Fluid", "AttackTrail_Spin", "AttackTrail_Wind", "Curve0", "Curve1" , "Curve2", "Helix27",
                                    "Lupers_Spear", "Particle_001", "ShockWave_Flow", "Spiral_001", "Wind_001",
                                    "IN_Spiral_02", "Swirl_Spine_X", "SwirlHelix",
                                    "FastAtk_1", "FastAtk_2L", "FastAtk_2R", "FastAtk_3L", "FastAtk_3R", "Grapple_Atk_2", "CounterATK", "DodgeATK", "FastATK1","FastATK2_L", "FastATK2_R",  "FastATK3_L" ,  "FastATK3_R", "FastATK4",
                                    "StrongAtk0", "StrongAtk1", "FastAtk03_Slash", "GrappleAtk02_Slash", "StrongAtk03_Slash"
                                    , "Cylinder_003", "Cylinder_003_02", "Cylinder_003_Noise" , "Spine", "Circle_002" ,"Sphere","CircleTwist", "CircleTwist2", "Plane" , "circle001", "circle002", "Ice",  "Rock", "Crystal", "Ring_Twist"
                                    , "CircleMesh0", "CircleMesh1", "CircleMesh2", "CircleMesh3", "CircleMesh4", "CircleMesh5", "ViperHandTrail" , "Viper_Sphere_Verticle_Trail"
                                    , "blood0"  , "blood1"  , "blood2", "Tornado" };
    ImGui::Combo("Mesh Shape", reinterpret_cast<int*>(&m_sEditingData.iMeshTypeIdx), Meshes, IM_ARRAYSIZE(Meshes));

    ImGui::Checkbox("Do Mask Scrolling", &m_bIsMaskScrolling);
    if (m_bIsMaskScrolling)
    {
        ImGui::Indent();
        const char* MaskTexture[] = { "width0", "width1", "width2",  "width3",  "width4",  "width5",  "width6", "width7", "length0" ,  "length1",  "length2" ,  "length3",  "length4" ,  "length5",  "length6" };
        ImGui::Combo("Mask Textures", reinterpret_cast<int*>(&m_sEditingData.iMaskTextureIdx), MaskTexture, IM_ARRAYSIZE(MaskTexture));
        ImGui::InputFloat("Mask Scroll Speed : ", &m_sEditingData.fMaskScrollSpeed);
        ImGui::Checkbox("Is Vecrtical", &IsVerticalScroll);
        ImGui::Checkbox("Is Inverse Direction", &IsInverseScroll);
        ImGui::Unindent();
    }
    else
        m_sEditingData.fMaskScrollSpeed = 0.f;

    ImGui::Checkbox("Dissolve", &bIsDissolve);
    if (bIsDissolve)
    {
        ImGui::Indent();
        const char* DissolveTex[] = { "DissolveTexture0", "DissolveTexture1", "DissolveTexture2", "DissolveTexture3","DissolveTexture4", "Normal0", "Normal1", "RockNormal"};
        ImGui::Combo("Dissolve Texture", reinterpret_cast<int*>(&m_sEditingData.sDissolveData.iDissolveTextureIdx), DissolveTex, IM_ARRAYSIZE(DissolveTex));
        ImGui::InputFloat("Dissolve Edge Width : ", reinterpret_cast<_float*>(&m_sEditingData.sDissolveData.fDissolveEdgeWidth));
        ImGui::ColorEdit4("Edge Color", (float*)&m_sEditingData.sDissolveData.fDissolveEdgeColor);
        ImGui::Unindent(); 
    }

    ImGui::Checkbox("Turbulence", &bIsTurbulence);
    if (bIsTurbulence)
    {
        ImGui::Indent();
        const char* MaskTexture[] = { "texture0", "texture1", "texture2",  "texture3" };
        ImGui::Combo("Turbulence Textures", reinterpret_cast<int*>(&m_sEditingData.iTurbulenceTextureIdx), MaskTexture, IM_ARRAYSIZE(MaskTexture));
        ImGui::InputFloat("Turbulence Speed : ", &m_sEditingData.fTurbulenceSpeed);
        ImGui::InputFloat("Turbulence Sample Size: ", &m_sEditingData.fTurbulenceSampleSize);
        ImGui::Unindent();
    }

    ImGui::Checkbox("Fresnel", &IsFresnel);

    m_sEditingData.IsCircle = isCircle;
    m_sEditingData.bIsLoop = loop;
    m_sEditingData.bIsScrollInverse = IsInverseScroll;
    m_sEditingData.bIsScrollVertical = IsVerticalScroll;
    m_sEditingData.bIsFresnel = IsFresnel;
    m_sEditingData.bIsTurbulence = bIsTurbulence;
    m_sEditingData.sDissolveData.bIsDissolve = bIsDissolve;

    if (ImGui::Button("Apply"))
        Apply(&m_sEditingData);
    if (ImGui::Button("Revert"))
        RevertChanges();
}

void CEffect_Mesh_Instance::RevertChanges()
{
    m_sEditingData = m_sData;
}

void CEffect_Mesh_Instance::Reset()
{
    __super::Reset();
    m_pVIBufferCom->Reset();
    m_fAccTime = 0.f;
    m_bRunning = false;
}

void CEffect_Mesh_Instance::SetSpreadData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SPREAD_SPEED, data.fSpreadSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::SPREAD_SPEED), data.fDuration);
}

void CEffect_Mesh_Instance::SetRotateData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::ROTATION_SPEED, data.fRotationSpeed);
    m_pVIBufferCom->Setting_Pivot(data.fPivot);
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::ROTATION_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetTwinkleData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::SCALE_SPEED, data.fScaleSpeed);
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::SCALE_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetUpwardData(void* pArg)
{
    CEffect_Prefab::EFFECT_EVENT data = *static_cast<CEffect_Prefab::EFFECT_EVENT*>(pArg);
    m_pVIBufferCom->Setting_Speed(CVIBuffer_Mesh_Instance::SPEED_VALUE::UPWARD_SPEED, data.fUpwardSpeed);
    m_sData.bGravity = data.bGravity;
    SetData(ENUM_CLASS(CVIBuffer_Mesh_Instance::SPEED_VALUE::UPWARD_SPEED),data.fDuration);
}

void CEffect_Mesh_Instance::SetScrollData(void* pArg)
{
    m_fScrollSpeed.x = *static_cast<_float*>(pArg);
    m_fScrollSpeed.y = *(static_cast<_float*>(pArg) + 1);
}

void CEffect_Mesh_Instance::SetStop()
{
    m_pVIBufferCom->Setting_Loop(false);
}

HRESULT CEffect_Mesh_Instance::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Slash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
        TEXT("Com_TextureMask"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Dissolve"),
        TEXT("Com_TextureDissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Normal"),
        TEXT("Com_TextureNormal"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Mesh_Instance::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_ScrollSpeed", &m_sEditingData.iScrollSpeed, sizeof(_float2))))
        return E_FAIL; 

    _bool ScrollDir = m_sData.bIsScrollVertical;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollYDir", &ScrollDir)))
        return E_FAIL;
    
    _bool IsScrollInv = m_sData.bIsScrollInverse;
    if (FAILED(m_pShaderCom->Bind_Bool("g_MaskScrollInv", &IsScrollInv)))
        return E_FAIL;

    _bool IsDissolve = m_sData.sDissolveData.bIsDissolve;

    if (FAILED(m_pShaderCom->Bind_Bool("g_IsDissolve", &IsDissolve)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_IsNormal", &m_bIsNormal)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeWidth", &m_sEditingData.sDissolveData.fDissolveEdgeWidth, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EdgeColor", &m_sEditingData.sDissolveData.fDissolveEdgeColor, sizeof(_float4))))
        return E_FAIL;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskScrollSpeed", &m_sData.fMaskScrollSpeed, sizeof(_float))))
        return E_FAIL;
    
    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", m_sData.iTextureIdx)))
        return E_FAIL;
    
    if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_sData.iMaskTextureIdx)))
        return E_FAIL;

    if (!m_bIsNormal)
    {
        if (FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", m_sData.sDissolveData.iDissolveTextureIdx)))
            return E_FAIL; 
    }
    else
    { 
        _bool IsFresnel = m_sData.bIsFresnel;
        if (FAILED(m_pNormalTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_NormalTexture", m_sData.sDissolveData.iDissolveTextureIdx - 5)))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_Bool("g_IsFresnel", &IsFresnel)))
            return E_FAIL;
    }



    return S_OK;
}

void CEffect_Mesh_Instance::Apply(void* pArg)
{
    m_sData = *static_cast<PARTICLE_DESC*>(pArg);
    m_sData.iTurbulenceTextureIdx = 0;
    const char* format = "../../Client/Bin/Data/Effect/MeshTrail/Mesh%d.dat";

    char finalPathBuffer[MAX_PATH] = {};
    sprintf_s(finalPathBuffer, MAX_PATH, format, m_sData.iMeshTypeIdx);
    strcpy_s(m_sData.pFilePath, MAX_PATH, finalPathBuffer);

    const char* NoiseFormat = "../../Client/Bin/Resources/Effect/Noise/Noise%d.png";
    
    char finalNoisePathBuffer[MAX_PATH] = {};
    sprintf_s(finalNoisePathBuffer, MAX_PATH, NoiseFormat, m_sData.iTurbulenceTextureIdx);
    strcpy_s(m_sData.pNoiseFilePath, MAX_PATH, finalNoisePathBuffer);

    m_bIsNormal = (m_sData.sDissolveData.bIsDissolve && m_sData.sDissolveData.iDissolveTextureIdx > 4) ? 1 : 0;

    Safe_Release(m_pVIBufferCom);
    m_pVIBufferCom = CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, &m_sData);
    m_pVIBufferCom->Initialize_Clone(nullptr);
    m_iEffect_Type = 1;

    m_sEditingData = m_sData;
    m_fScrollSpeed = m_sData.iScrollSpeed;
    if (m_sData.fMaskScrollSpeed == 0)
        m_bIsMaskScrolling = false;
    else
        m_bIsMaskScrolling = true;
}

CEffect_Mesh_Instance* CEffect_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Mesh_Instance* pInstance = new CEffect_Mesh_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Mesh_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Mesh_Instance::Clone(void* pArg)
{
    CEffect_Mesh_Instance* pInstance = new CEffect_Mesh_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Mesh_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Mesh_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMaskTextureCom);
    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pNormalTextureCom);
}




