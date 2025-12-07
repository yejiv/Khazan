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
    void    Update(_float fTimeDelta);

public:
	HRESULT Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable);
	void    Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc);
	void    Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition);
	void    Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable);
	_bool   Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex);
	void    Clear(_uint iLevelIndex);

public:
    void    Start_LightTransition(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_TRANSITION_DESC& Desc, _bool isRestore);
    void    Backup_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);

public:
	const LIGHT_DESC*       Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);
    const vector<_wstring>& Get_LightTags(_uint iLevelIndex);

private:
	class CGameInstance*                    m_pGameInstance = { nullptr };

	unordered_map<_wstring, class CLight*>* m_pLights = { nullptr };
	_uint                                   m_iNumLevels = {};
    vector<_wstring>                        m_strLightTags = {}; // For Light Setting

    // Transition
    LIGHT_TRANSITION_DESC                   m_TargetLightDesc = {};
    _float                                  m_fTransTimeAcc = {};
    _bool                                   m_isTransition = {};
    class CLight*                           m_pTransLight = { nullptr };
    LIGHT_DESC                              m_StartLightDesc;
    _float                                  m_fBlinkPeriod = {};

    // Backup
    map<_wstring, LIGHT_DESC>               m_OriginalLightDesc;

private:
	class CLight*           Find_Light(const _wstring& strLightTag, _uint iLevelIndex);

public:
	static CLight_Manager*  Create(_uint iNumLevels);
	virtual void            Free() override;
};

NS_END