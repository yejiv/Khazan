#include "Editor_MeshMaterial.h"
#include "Shader.h"

CEditor_MeshMaterial::CEditor_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CEditor_MeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	string filePath(pModelFilePath);
	_bool isGLTF = (filePath.substr(filePath.find_last_of(".") + 1) == "gltf");

	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint	iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));
		m_Material_Data.iNumTextures.push_back(iNumTextures);

		vector<string> tempExts;
		vector<string> tempFullPaths;


		for (_uint j = 0; j < iNumTextures; j++)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };

			aiString	strTexturePath;
			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				break;

			_char			szFullPath[MAX_PATH] = {};
			_char			szDrive[MAX_PATH] = {};
			_char			szDir[MAX_PATH] = {};
			_char			szFileName[MAX_PATH] = {};
			_char			szExt[MAX_PATH] = {};

		
			if (isGLTF)
			{	
				// .gltf 방식 
				string texturePath = strTexturePath.data;

				_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

				strcpy_s(szFullPath, szDrive);
				strcat_s(szFullPath, szDir);
				strcat_s(szFullPath, texturePath.c_str());

				size_t dotPos = texturePath.find_last_of('.');
				if (dotPos != string::npos)
				{
					string ext = texturePath.substr(dotPos);
					strcpy_s(szExt, ext.c_str());
				}
				//string message = "Mat Name : " + string(szFullPath) + "\n";
				//OutputDebugStringA(message.c_str());
			}
			else
			{
				//.fbx 방식
				_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
				_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

				strcpy_s(szFullPath, szDrive);
				strcat_s(szFullPath, szDir);
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				//string message = "Mat Name : " + string(szFileName) + "\n";
				//OutputDebugStringA(message.c_str());
			}

			_tchar			szFullPathW[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szFullPathW, MAX_PATH);

			HRESULT     hr = {};

			if (false == strcmp(szExt, ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szFullPathW, nullptr, &pSRV);
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;

			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szFullPathW, nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_SRVs[i].push_back(pSRV);

			tempExts.push_back(static_cast<string>(szExt));
			tempFullPaths.push_back(static_cast<string>(szFullPath));


		}

		m_Material_Data.vecExts.push_back(tempExts);
		m_Material_Data.vecFullPaths.push_back(tempFullPaths);

	}


	return S_OK;
}

HRESULT CEditor_MeshMaterial::Bind_Resources(CShader* pShader, const _char* pConstantName, _uint iTextureIndex, _uint iTextureType)
{
	if (iTextureType >= AI_TEXTURE_TYPE_MAX && iTextureIndex >= m_SRVs[iTextureType].size())
		return E_FAIL;

	return 	pShader->Bind_SRV(pConstantName, m_SRVs[iTextureType][iTextureIndex]);
}

CEditor_MeshMaterial* CEditor_MeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CEditor_MeshMaterial* pInstance = new CEditor_MeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX(TEXT("Failed to Created : CEditor_MeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor_MeshMaterial::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& vecSRV : m_SRVs)
	{
		for (auto pSRV : vecSRV)
			Safe_Release(pSRV);
		vecSRV.clear();
	}
}
