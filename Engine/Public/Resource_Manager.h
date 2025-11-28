#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CResource_Manager final : public CBase
{
    typedef struct TagTexture {
        _uint iPrototypeLevelIndex;
        _wstring strPrototypeTag;
        _tchar* pTextureFilePath;
        _uint iNumTexture = 1;
        void* pArg = nullptr;
    }TEXTURE_DESC;

    typedef struct TagModel {
        _uint iPrototypeLevelIndex;
        _wstring strPrototypeTag;
        _char* pModelFilePath;
        MODELTYPE eModelType;
        _matrix PreTransformMatrix;
        void* pArg = nullptr;
    }MODEL_DESC;

private:
    CResource_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CResource_Manager() = default;

public:
    HRESULT Initialize();

public:
    HRESULT Add_Texture(_wstring strTextureTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, _tchar* pTextureFilePath, _uint iNumTexture = 1, void* pArg = nullptr);
    HRESULT Add_Model(_wstring strModelTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, MODELTYPE eModelType, _char* pModelFilePath, _matrix PreTransformMatrix, void* pArg = nullptr);

public:
    class CTexture* Clone_Texture(_wstring strTextureTag);
    class CModel* Clone_Model(_wstring strModelTag);

public:
    class CTexture* Get_Texture(_wstring strTextureTag);
    class CModel* Get_Model(_wstring strModelTag);

public:
    void Switch_Texture(_wstring strTextureTag, class CGameObject* pGameObject, class CTexture** pTexture, _wstring strComponentTag);

public:
    _bool Push_MeshMetrial_SRV(string strFileName, ID3D11ShaderResourceView* pResource);
    _bool Exist_MeshMetrial_SRV_InCache(string strFileName);
    string Convert_FullPath(string strFullPath);
    ID3D11ShaderResourceView* Get_MeshMetrial_SRVFromCache(string strFileName);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    map<_wstring, TEXTURE_DESC> m_pTextureDescs;
    map<_wstring, class CTexture*> m_pTextures;

    map<_wstring, MODEL_DESC> m_pModelDescs;
    map<_wstring, class CModel*> m_pModels;


    map<string, ID3D11ShaderResourceView*> m_pMetrialSRVCache;

public:
    static CResource_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;

};

NS_END