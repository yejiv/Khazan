#include "Prop_Interactive.h"

#include "GameInstance.h"

CProp_Interactive::CProp_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Interactive::CProp_Interactive(const CProp_Interactive& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Interactive::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Interactive::Initialize_Clone(void* pArg)
{
    PROP_INTERACTIVE_DESC* pDesc = static_cast<PROP_INTERACTIVE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    memcpy(m_szInteractiveTag, pDesc->szInteractiveTag, MAX_PATH);

    m_iEventID = pDesc->iEventID;

    // 맵 오브젝트의 월드 행렬 갱신 ( 파일 입출력 받은걸로 )
    m_pTransformCom->Set_WorldMatrix_4x4(pDesc->WorldMatrix);

    return S_OK;
}

void CProp_Interactive::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CProp_Interactive::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CProp_Interactive::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CProp_Interactive::Render()
{
    return S_OK;
}

HRESULT CProp_Interactive::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 맵 오브젝트 ID 쉐이더에 바인딩
    //CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_iMapObjectID", &m_iMapObjectID, sizeof(_uint)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Interactive::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CProp_Interactive::SoundStop(const _tchar* pSoundKey)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->StopByKey(wstrAddWav.c_str());
}

void CProp_Interactive::SoundStop_FadeOut(const _tchar* pSoundKey, _float fFadeTime)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->StopByKey_FadeOut(wstrAddWav.c_str(), fFadeTime);
}

_bool CProp_Interactive::IsPlayingSound(const _tchar* pSoundKey)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    return m_pGameInstance->IsPlayingByKey(wstrAddWav.c_str());
}

void CProp_Interactive::Sound_FadeIn(const _tchar* pSoundKey, _float fVolume, _float fFadeTime, _bool isLoop, FMOD_CHANNEL** ppOutChannel)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySound_FadeIn(wstrAddWav.c_str(), fVolume, fFadeTime, isLoop, ppOutChannel);
}

void CProp_Interactive::SoundOnce(const _tchar* pSoundKey, _float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundOnce(wstrAddWav.c_str(), fVolume, ppOutChannel);
}

void CProp_Interactive::SoundLoop(const _tchar* pSoundKey, _float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundLoop(wstrAddWav.c_str(), fVolume, ppOutChannel);
}

void CProp_Interactive::SoundOnce(const _tchar* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, _float fVolume)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundOnce(wstrAddWav.c_str(), vPos, ppOutChannel, fVolume);
}

void CProp_Interactive::SoundOnce(const _tchar* pSoundKey, _vector vPos, _float3 vVel, _float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundOnce(wstrAddWav.c_str(), vPos, vVel, fVolume, ppOutChannel);
}

void CProp_Interactive::SoundLoop(const _tchar* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, _float fVolume)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundLoop(wstrAddWav.c_str(), vPos, ppOutChannel, fVolume);
}

void CProp_Interactive::SoundLoop(const _tchar* pSoundKey, _vector vPos, _float3 vVel, _float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    _wstring wstrAddWav = pSoundKey;
    wstrAddWav += TEXT(".wav");

    m_pGameInstance->PlaySoundLoop(wstrAddWav.c_str(), vPos, vVel, fVolume, ppOutChannel);
}

void CProp_Interactive::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
