#include "Texture_Atlas.h"
#include "Shader.h"

CTexture_Atlas::CTexture_Atlas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent{ pDevice, pDeviceContext }
{
}

CTexture_Atlas::CTexture_Atlas(const CTexture_Atlas& Prototype)
	: CComponent( Prototype )
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_SRVs{ Prototype.m_SRVs }
	, m_AtlasDatas{Prototype.m_AtlasDatas}
	, m_TexSizeX{ Prototype.m_TexSizeX }
	, m_TexSizeY{ Prototype.m_TexSizeY }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture_Atlas::Initialize_Prototype(const _tchar* pAtlasDataFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar          szDirve[MAX_PATH] = {};
	_tchar          szDir[MAX_PATH] = {};
	_tchar          szExt[MAX_PATH] = {};

	_wsplitpath_s(pAtlasDataFilePath, szDirve, MAX_PATH, szDir, MAX_PATH, nullptr, 0, szExt, MAX_PATH);

	for (_uint i = 0; i < iNumTextures; i++)
	{
		_tchar szFullPath[MAX_PATH] = {};

		wsprintf(szFullPath, pAtlasDataFilePath, i);

		ifstream File(szFullPath);
		if (!File.is_open())
		{
			MSG_BOX(TEXT("failed to Open : AtlasData"));
			return E_FAIL;
		}

		nlohmann::json jsonData;
		File >> jsonData;
		unordered_map<string, ATLASFRAMEDATA> mapAtlasDatas;
		for (const auto& frame : jsonData["frames"])
		{
			string szFrameName = frame["filename"].get<string>();

			ATLASFRAMEDATA vFrameData = {};

			vFrameData.iLeft = frame["frame"]["x"].get<_int>();
			vFrameData.iTop = frame["frame"]["y"].get<_int>();
			vFrameData.iSizeX = frame["frame"]["w"].get<_int>();
			vFrameData.iSizeY = frame["frame"]["h"].get<_int>();

			mapAtlasDatas.emplace(szFrameName, vFrameData);
		}
		m_AtlasDatas.push_back(mapAtlasDatas);

		string szImageName = jsonData["meta"]["image"].get<string>();
		_int iTexSizeX = jsonData["meta"]["size"]["w"].get<_int>();
		_int iTexSizeY = jsonData["meta"]["size"]["h"].get<_int>();

		m_TexSizeX.push_back(iTexSizeX);
		m_TexSizeY.push_back(iTexSizeY);

		_wstring szFileFullName = {};
		szFileFullName = CharToWString(szImageName.c_str());

		_wsplitpath_s(szFileFullName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		_wstring szImageFullPath = _wstring(szDirve) + _wstring(szDir) + szFileFullName;

		ID3D11ShaderResourceView* pSPV = { nullptr };

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szImageFullPath.c_str(), nullptr, &pSPV)))
				return E_FAIL;
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
			MSG_BOX(TEXT("Ext is Tga"));
			return E_FAIL;
		}
		else
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szImageFullPath.c_str(), nullptr, &pSPV)))
				return E_FAIL;

		m_SRVs.push_back(pSPV);
	}

	return S_OK;
}

HRESULT CTexture_Atlas::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CTexture_Atlas::Bind_Shader_Texture(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture_Atlas::Bind_Shader_AllTexture(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}

_float4 CTexture_Atlas::FindTexFrame(const string pFrameName, _uint iTextureIndex)
{
	_float4 vFrame = { 0.f, 0.f, 1.f, 1.f };
	if (iTextureIndex >= m_iNumTextures)
		return vFrame;

	auto vFindFrame = m_AtlasDatas[iTextureIndex].find(pFrameName);

	if (vFindFrame == m_AtlasDatas[iTextureIndex].end())
		return vFrame;

	ATLASFRAMEDATA TextUV = vFindFrame->second;
	_float fTexelx = (1.0f / m_TexSizeX[iTextureIndex]) * 3.f;
	_float fTexely = (1.0f / m_TexSizeY[iTextureIndex]) * 3.f;


	vFrame.x = _float(TextUV.iLeft) / m_TexSizeX[iTextureIndex] - fTexelx;
	vFrame.y = _float(TextUV.iTop) / m_TexSizeY[iTextureIndex] - fTexely;
	vFrame.z = (_float(TextUV.iLeft) + TextUV.iSizeX) / m_TexSizeX[iTextureIndex] + fTexelx;
	vFrame.w = (_float(TextUV.iTop) + TextUV.iSizeY) / m_TexSizeY[iTextureIndex] + fTexely;

	return vFrame;

}

_float2 CTexture_Atlas::FindTexSize(const string pFrameName, _uint iTextureIndex)
{
	_float2 vSize = { 10.f, 10.f };

	if (iTextureIndex >= m_iNumTextures)
		return vSize;

	auto vFindFrame = m_AtlasDatas[iTextureIndex].find(pFrameName);

	if (vFindFrame == m_AtlasDatas[iTextureIndex].end())
	{
		MSG_BOX(TEXT("Find Tex Failed"));
		return vSize;
	}
	vSize.x = vFindFrame->second.iSizeX + 6.f;
	vSize.y = vFindFrame->second.iSizeY + 6.f;
	return vSize;
}

CTexture_Atlas* CTexture_Atlas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pAtlasDataFilePath, _uint iNumTextures)
{
	CTexture_Atlas* pInstance = new CTexture_Atlas(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pAtlasDataFilePath, iNumTextures)))
	{
		MSG_BOX(TEXT("Failed Created : CTexture_Atlas"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTexture_Atlas::Clone(void* pArg)
{
	CTexture_Atlas* pInstance = new CTexture_Atlas(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTexture_Atlas"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture_Atlas::Free()
{
	__super::Free();


	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);
	m_SRVs.clear();

}





