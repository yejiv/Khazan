#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCamera_Manager final : public CBase
{
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Camera(_uint iLevelIndex, class CCamera* pCamera);
	void Change_Camera(_uint iLevelIndex, _uint iCameraType);
	void Change_Camera(_uint iLevelIndex, _wstring strCameraTag);
	class CCamera* Find_Camera(_uint iLevelIndex, _uint iCameraType);
	void Clear(_uint iNumLevels);

public:
	vector<class CCamera*> Get_pCameras(_uint iNumLevel) { return m_pCameras[iNumLevel]; }
	class CCamera* Get_ActiveCamera() { return m_pActiveCamera; }

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = {};
	vector<class CCamera*>* m_pCameras;
	class CCamera* m_pActiveCamera = { nullptr };

public:
	static CCamera_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END