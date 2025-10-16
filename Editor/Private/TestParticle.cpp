#include "TestParticle.h"
#include "GameInstance.h"

CTestParticle::CTestParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTestParticle::CTestParticle(const CTestParticle& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTestParticle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTestParticle::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // Compute Shader Performance Test
    QueryPerformanceFrequency(&m_Freq);

    return S_OK;
}

void CTestParticle::Priority_Update(_float fTimeDelta)
{
}

void CTestParticle::Update(_float fTimeDelta)
{
    // =============== GPU Spread ===============

    //  QueryPerformanceCounter(&m_StartGpu);
    //  
    //  m_pVIBufferCom->Spread(fTimeDelta);
    //  
    //  QueryPerformanceCounter(&m_EndGpu);
    //  
    //  _float fGPUElapsed = (m_EndGpu.QuadPart - m_StartGpu.QuadPart) * 1000.0f / static_cast<_float>(m_Freq.QuadPart);
    //  m_TotalGpuTime += fGPUElapsed;
    //  m_FrameCount++;
    //  
    //  if (m_FrameCount >= m_iPrintInterval)
    //  {
    //      double avgGpu = m_TotalGpuTime / m_FrameCount;
    //  
    //      std::wcout.setf(std::ios::fixed, std::ios::floatfield);
    //      std::wcout.precision(3);
    //      std::wcout << L"[Avg over " << m_FrameCount << L" frames] "
    //          << L"GPU Drop: " << avgGpu << L" ms"
    //          << std::endl;
    //  
    //      m_FrameCount = 0;
    //      m_TotalGpuTime = 0.0;
    //  }

    // =============== CPU Spread ===============

    //  QueryPerformanceCounter(&m_StartCpu);
    //  
    //  m_pVIBufferCom->Spread(fTimeDelta);
    //  
    //  QueryPerformanceCounter(&m_EndCpu);
    //  
    //  _float fCPUElapsed = (m_EndCpu.QuadPart - m_StartCpu.QuadPart) * 1000.0f / static_cast<_float>(m_Freq.QuadPart);
    //  m_TotalCpuTime += fCPUElapsed;
    //  m_FrameCount++;
    //  
    //  if (m_FrameCount >= m_iPrintInterval)
    //  {
    //      double avgCpu = m_TotalCpuTime / m_FrameCount;
    //  
    //      std::wcout.setf(std::ios::fixed, std::ios::floatfield);
    //      std::wcout.precision(3);
    //      std::wcout << L"[Avg over " << m_FrameCount << L" frames] "
    //          << L"CPU Drop: " << avgCpu << L" ms"
    //          << std::endl;
    //  
    //      m_FrameCount = 0;
    //      m_TotalCpuTime = 0.0;
    //  }

    m_pVIBufferCom->Move(fTimeDelta);
}

void CTestParticle::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CTestParticle::Render()
{
    //  if (FAILED(Bind_ShaderResources()))
    //      return E_FAIL;
    //  
    //  m_pShaderCom->Begin(0);
    //  
    //  m_pVIBufferCom->Bind_Resources();
    //  
    //  m_pVIBufferCom->Render();

    //  _uint           iNumMeshes = m_pVIBufferCom->Get_NumMeshes();
    //  
    //  for (size_t i = 0; i < iNumMeshes; i++)
    //  {
    //      if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
    //          return E_FAIL;
    //  
    //      m_pShaderCom->Begin(0);
    //  
    //      m_pModelCom->Render(i);
    //  }


    // 텍스처 컴포넌트 바인드
    //  if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
    //      return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTestParticle::Ready_Components()
{
    //  if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
    //      TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //      return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_MeshParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Particle_Spread"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_TestParticle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestParticle::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    //  if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
    //      return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CTestParticle* CTestParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestParticle* pInstance = new CTestParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CTestParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestParticle::Clone(void* pArg)
{
    CTestParticle* pInstance = new CTestParticle(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTestParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestParticle::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
