#include "Decal_Manager.h"
#include "Decal.h"
#include "GameInstance.h"

CDecal_Manager::CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDecal_Manager::Initialize()
{
    // 컴포넌트 생성
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDecal_Manager::Update(_float fTimeDelta)
{
    // 죽은 데칼 순회, 풀에 반납하고 컨테이너에서 지우기
    for (auto iter = m_Decals.begin(); iter != m_Decals.end();)
    {
        if ((*iter)->Get_IsDead() && (*iter)->Get_IsPool())
        {
            iter = m_Decals.erase(iter);
        }
        else
            ++iter;
    }
}

HRESULT CDecal_Manager::Render()
{
    // 데칼의 월드, 카메라 뷰 투영, 뷰 역행렬, 투영 역행렬 바인딩
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    // 디퓨즈, 뎁스, 노말
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;

    _uint           iNumViewports = { 1 };
    D3D11_VIEWPORT  ViewportDesc{};
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    // 스크린 사이즈
    _float2 vScreenSize = _float2(ViewportDesc.Width, ViewportDesc.Height);
    if (FAILED(m_pShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_float2))))
        return E_FAIL;

    // 활성화된 데칼 개수만큼 순회, 해당 데칼의 월드, 뷰, 투영 바인딩
    for (auto& pDecal : m_Decals)
    {
        if (FAILED(pDecal->Bind_ShaderResources(m_pShader, m_pTexture, m_pVIBuffer)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CDecal_Manager::Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const DECAL_DESC& Desc)
{
    // 풀 태그, 레벨 인덱스, 레이어 태그, 포지션, 노말, 스케일
    // 인자로 받아 풀에서 꺼냄
    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(iLayerLevelIndex, strPoolTag);
    if (nullptr == pGameObject)
        return E_FAIL;

    CDecal* pDecal = dynamic_cast<CDecal*>(pGameObject);
    if (nullptr == pDecal)
        return E_FAIL;

    // 세팅
    pDecal->Set_Desc(Desc);
    _uint iNumTextures = m_pTexture[ENUM_CLASS(Desc.eType)]->Get_NumTextures();
    _uint iTextureIndex = static_cast<_uint>(m_pGameInstance->Rand(0.f, static_cast<_float>(iNumTextures)));
    pDecal->Set_TextureIndex(iTextureIndex);
    pDecal->Set_RandomSeed(static_cast<_uint>(m_pGameInstance->Rand(0.f, 256.f)));
    
    // Test
    pDecal->Set_EnableDecoration(true);

    // 컨테이너에 저장
    m_Decals.push_back(pDecal);

    // 레이어에 추가
    m_pGameInstance->Push_PoolObject_ToLayer(iLayerLevelIndex, strLayerTag, pDecal);

    return S_OK;
}

HRESULT CDecal_Manager::Ready_Components()
{
    // Engine_Shader_Decal.hlsl 생성
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Decal.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    // VIBuffer Cube 생성
    m_pVIBuffer = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    // Decal Texture 생성
    
    // =============== Linear ===============
    vector<const _tchar*> TextureTags;
    TextureTags =
    {
        TEXT("FT_Decal_Blood_000.png"),
        TEXT("FT_Decal_Blood_001.png"),
        TEXT("FT_Decal_Blood_002.png"),
        TEXT("FT_Decal_Blood_003.png"),
        TEXT("FT_Decal_Blood_A_000.png"),
        TEXT("FT_Decal_Blood_A_001.png"),
        TEXT("FT_Decal_Blood_A_002.png"),
        TEXT("FT_Decal_Blood_A_003.png"),
        TEXT("FT_Decal_Blood_Linear_000.png"),
        TEXT("FT_Decal_Dirt_000.png"),
        TEXT("FT_Decal_Dirt_001.png"),
        TEXT("FT_Decal_Dirt_002.png"),
        TEXT("FT_Decal_Dirt_003.png"),
    };

    m_pTexture[ENUM_CLASS(DECALTYPE::LINEAR)] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Decal/"), TextureTags);
    if (nullptr == m_pTexture)
        return E_FAIL;

    // =============== Circle ===============
    TextureTags =
    {
        TEXT("FT_Decal_Blood_004.png"),
        TEXT("FT_BloodDecal_A_004.png"),
        TEXT("FT_BloodDecal_A_005.png"),
        TEXT("FT_BloodDecal_A_007.png"),
    };

    m_pTexture[ENUM_CLASS(DECALTYPE::CIRCLE)] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Decal/"), TextureTags);
    if (nullptr == m_pTexture)
        return E_FAIL;

    // =============== Curve ===============
    TextureTags =
    {
        TEXT("FT_Decal_Blood_Spin_001.png"),
        TEXT("FT_Decal_Blood_Curve_A_000.png"),
        TEXT("FT_Decal_Dirt_Curve_000.png"),
    };

    m_pTexture[ENUM_CLASS(DECALTYPE::CURVE)] = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Decal/"), TextureTags);
    if (nullptr == m_pTexture)
        return E_FAIL;

    return S_OK;
}

CDecal_Manager* CDecal_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecal_Manager* pInstance = new CDecal_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CDecal_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecal_Manager::Free()
{
    __super::Free();

    for (auto& pDecal : m_Decals)
        Safe_Release(pDecal);
    m_Decals.clear();

    for (_uint i = 0; i < ENUM_CLASS(DECALTYPE::END); ++i)
        Safe_Release(m_pTexture[i]);

    Safe_Release(m_pVIBuffer);
    Safe_Release(m_pShader);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
