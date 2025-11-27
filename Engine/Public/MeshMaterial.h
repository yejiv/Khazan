#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMeshMaterial final : public CBase
{
private:
	CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMeshMaterial() = default;

public:
	HRESULT Initialize(MATERIAL_DATA& data);
    HRESULT Initialize_V2(MATERIAL_DATA& data);

	HRESULT Bind_Resources(class CShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iIndex);
	HRESULT Bind_Resources(class CDeferredShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iIndex);
	HRESULT Deferred_Bind_Resources(ID3D11DeviceContext* pDeferredContext, class CShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iIndex);
    HRESULT Deferred_Bind_Resources_V2(ID3D11DeviceContext* pDeferredContext, CDeferredShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iIndex);
private:
	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };

	vector<ID3D11ShaderResourceView*>			m_SRVs[TEXTURETYPE_MAX];
	map<_wstring, _uint>						m_TextureCache;   

    class CGameInstance*                        m_pGameInstance = { nullptr };

private:
	_bool		ExistTextureCache(_wstring strPath);

public:
	static CMeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MATERIAL_DATA& data, _bool isSRVCache = false);
	virtual void Free() override;
};

NS_END

