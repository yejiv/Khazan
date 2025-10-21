#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);

public:
	HRESULT Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable);
	void Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc);
	void Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition);
	void Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable);
	_bool Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex);
	void Clear(_uint iLevelIndex);

public:
	const LIGHT_DESC* Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	unordered_map<_wstring, class CLight*>* m_pLights = { nullptr };
	_uint m_iNumLevels = {};

private:
	class CLight* Find_Light(const _wstring& strLightTag, _uint iLevelIndex);

public:
	static CLight_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END