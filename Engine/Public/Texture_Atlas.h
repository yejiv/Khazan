#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CShader;

class ENGINE_DLL CTexture_Atlas final : public CComponent
{
public:
	typedef struct AtlasFrameData
	{
		_int iLeft;
		_int iTop;
		_int iSizeX;
		_int iSizeY;
	}ATLASFRAMEDATA;
private:
	CTexture_Atlas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTexture_Atlas(const CTexture_Atlas& Prototype);
	virtual ~CTexture_Atlas() = default;

public:
	virtual HRESULT								Initialize_Prototype(const _tchar* pAtlasDataFilePath, _uint iNumTextures);
	virtual HRESULT								Initialize_Clone(void* pArg) override;

public:
	HRESULT										Bind_Shader_Texture(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex = 0);
	HRESULT										Bind_Shader_AllTexture(class CShader* pShader, const _char* pConstantName);

	_float4										FindTexFrame(const string pFrameName, _uint iTextureIndex = 0);
	_float2										FindTexSize(const string pFrameName, _uint iTextureIndex = 0);

private:
	vector<ID3D11ShaderResourceView*>			m_SRVs;
	vector<
		unordered_map<string, ATLASFRAMEDATA>>	m_AtlasDatas;
	_int										m_iTexSizeX = {};
	_int										m_iTexSizeY = {};
	_uint										m_iNumTextures = {};

public:
	static CTexture_Atlas*						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pAtlasDataFilePath, _uint iNumTextures = 1);
	virtual CComponent*							Clone(void* pArg) override;
	virtual void								Free() override;
};

NS_END
