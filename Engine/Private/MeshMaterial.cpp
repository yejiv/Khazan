
#include "MeshMaterial.h"
#include "Shader.h"

CMeshMaterial::CMeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CMeshMaterial::Initialize(MATERIAL_DATA& data)
{
	//_char currentDir[MAX_PATH];
	//GetCurrentDirectoryA(MAX_PATH, currentDir);
	//OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());

	//char exePath[MAX_PATH];
	//GetModuleFileNameA(NULL, exePath, MAX_PATH);
	//std::filesystem::path editorExeDir = std::filesystem::path(exePath).parent_path();

	//OutputDebugStringA(("[Client.exe Dir] " + editorExeDir.string() + "\n").c_str());

	for (_uint i = 0; i < TEXTURETYPE_MAX; i++)
	{
		_uint	iNumTextures = data.iNumTextures[i];

		for (_uint j = 0; j < iNumTextures; j++)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };

			// 파일 경로 유효성 검사 추가 (유효하지 않은 경로는 건너뛰기)
			if (data.vecFullPaths[i].empty() || j >= data.vecFullPaths[i].size())	continue;

			/* 파일주소가 있는지 */
			string fullPath = data.vecFullPaths[i][j];
			if (fullPath.empty()) continue;

			/* 이미 저장한 텍스쳐인지 */
			if (ExistTextureCache(AnsiToWString(fullPath)))continue;

			HRESULT     hr = {};

			_tchar			szFullPathW[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, data.vecFullPaths[i][j].c_str(), static_cast<DWORD>(strlen(data.vecFullPaths[i][j].c_str())), szFullPathW, MAX_PATH);

			if (false == strcmp(data.vecExts[i][j].c_str(), ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szFullPathW, nullptr, &pSRV);
			}
			else if (false == strcmp(data.vecExts[i][j].c_str(), ".tga"))
				return E_FAIL;

			else
			{
				try
				{
					hr = CreateWICTextureFromFile(m_pDevice, szFullPathW, nullptr, &pSRV);

					if (FAILED(hr))
					{
						OutputDebugStringA((" [CMeshMaterial::Initialize] WIC 텍스처 로딩 실패: " + fullPath + "\n").c_str());
						return hr;
					}
				}
				catch (const std::exception& e)
				{
					OutputDebugStringA(e.what());
				}

			}

			if (FAILED(hr))
			{
				OutputDebugStringA((" [CMeshMaterial::Initialize] 텍스처 로딩 실패: " + fullPath + "\n").c_str());
				return E_FAIL;
			}

			m_SRVs[i].push_back(pSRV);
			m_TextureCache.emplace(AnsiToWString(fullPath), 1);

		}
	}

	return S_OK;
}

HRESULT CMeshMaterial::Bind_Resources(class CShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iIndex)
{
	if (iIndex >= m_SRVs[iTextureType].size())
		return E_FAIL;

	//OutputDebugStringA(to_string(iIndex).c_str());
	/*for (auto cache : m_TextureCache)
		OutputDebugStringW(cache.first.c_str());*/
	//OutputDebugStringA("\n");

	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureType][iIndex]);
}

_bool CMeshMaterial::ExistTextureCache(_wstring strPath)
{
	auto iter = m_TextureCache.find(strPath);
	if (iter != m_TextureCache.end())
		return true;
	return false;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MATERIAL_DATA& data)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(data)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMeshMaterial::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}
}
