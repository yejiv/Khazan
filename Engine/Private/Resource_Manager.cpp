#include "Resource_Manager.h"
#include "GameInstance.h"
#include "Texture.h"
#include "GameObject.h"
#include "Component.h"

CResource_Manager::CResource_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance { CGameInstance::GetInstance()}
    , m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CResource_Manager::Initialize()
{
    return S_OK;
}

HRESULT CResource_Manager::Add_Texture(_wstring strTextureTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, _tchar* pTextureFilePath, _uint iNumTexture, void* pArg)
{
    if (FAILED(m_pGameInstance->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, pTextureFilePath, iNumTexture))))
        return E_FAIL;

    TEXTURE_DESC TDesc{};
    TDesc.iNumTexture = iNumTexture;
    TDesc.iPrototypeLevelIndex = iPrototypeLevelIndex;
    TDesc.pTextureFilePath = pTextureFilePath;
    TDesc.strPrototypeTag = strPrototypeTag;
    TDesc.pArg = pArg;
    
    m_pTextureDescs.emplace(strTextureTag, TDesc);
    
    m_pTextures.emplace(strTextureTag, dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, TDesc.iPrototypeLevelIndex, TDesc.strPrototypeTag, TDesc.pArg)));

    return S_OK;
}

HRESULT CResource_Manager::Add_Model(_wstring strModelTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, MODELTYPE eModelType, _char* pModelFilePath, _matrix PreTransformMatrix, void* pArg)
{
    if (FAILED(m_pGameInstance->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag,
        CModel::Create(m_pDevice, m_pContext, eModelType, pModelFilePath, PreTransformMatrix))))
        return E_FAIL;

    MODEL_DESC MDesc{};
    MDesc.eModelType = eModelType;
    MDesc.iPrototypeLevelIndex = iPrototypeLevelIndex;
    MDesc.pModelFilePath = pModelFilePath;
    MDesc.PreTransformMatrix = PreTransformMatrix;
    MDesc.strPrototypeTag = strPrototypeTag;
    MDesc.pArg = pArg;

    m_pModelDescs.emplace(strModelTag, MDesc);

    m_pModels.emplace(strModelTag, dynamic_cast<CModel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, MDesc.iPrototypeLevelIndex, MDesc.strPrototypeTag, MDesc.pArg)));

    return S_OK;
}

CTexture* CResource_Manager::Clone_Texture(_wstring strTextureTag)
{
    auto iter = m_pTextureDescs.find(strTextureTag);

    if (iter == m_pTextureDescs.end())
        return nullptr;

    return dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iter->second.iPrototypeLevelIndex, iter->second.strPrototypeTag, iter->second.pArg));
}

CModel* CResource_Manager::Clone_Model(_wstring strModelTag)
{
    auto iter = m_pModelDescs.find(strModelTag);

    if (iter == m_pModelDescs.end())
        return nullptr;

    return dynamic_cast<CModel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iter->second.iPrototypeLevelIndex, iter->second.strPrototypeTag, iter->second.pArg));
}

CTexture* CResource_Manager::Get_Texture(_wstring strTextureTag)
{
    auto iter = m_pTextures.find(strTextureTag);

    if (iter == m_pTextures.end())
        return nullptr;

    return iter->second;
}

CModel* CResource_Manager::Get_Model(_wstring strModelTag)
{
    auto iter = m_pModels.find(strModelTag);

    if (iter == m_pModels.end())
        return nullptr;

    return iter->second;
}

void CResource_Manager::Switch_Texture(_wstring strTextureTag, CGameObject* pGameObject, CTexture** pTexture, _wstring strComponentTag)
{
    pGameObject->Remove_Component(strComponentTag);

    Safe_Release(*pTexture);

    *pTexture = Clone_Texture(strTextureTag);
}

CResource_Manager* CResource_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CResource_Manager* pInstance = new CResource_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CResource_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CResource_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    m_pTextureDescs.clear();

    for (auto pTexture : m_pTextures)
    {
        Safe_Release(pTexture.second);
    }
    m_pTextures.clear();

    m_pModelDescs.clear();

    for (auto pModel : m_pModels)
    {
        Safe_Release(pModel.second);
    }
    m_pModels.clear();

}

