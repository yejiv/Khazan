#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CCamera;
class CGameInstance;
NS_END

NS_BEGIN(Client)

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
	class CCamera* Find_Camera(_uint iLevelIndex, CAMERATYPE eCameraType);
	class CCamera* Find_Camera(_uint iLevelIndex, _wstring strCameraTag);
	void Clear(_uint iNumLevels);

public:
	vector<class CCamera*> Get_pCameras(_uint iNumLevel) { return m_pCameras[iNumLevel]; }
	class CCamera* Get_ActiveCamera() { return m_pActiveCamera; }
	_float3 Get_ActiveCameraPos();
	_float4 Get_ActiveCameraLook();

public:
	void ActiveCamera_Shaking(_float fPower, _float fDuration);
	void ActiveCamera_PushFOVModifier(const FOVModifier& tNewModifier);
	void ActiveCamera_KillFov(const _wstring& strID);
    void Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir);
    void ActiveCamera_InteractMove();
    void DeactivateCamera_InteractMove();
    void Yetuga_Holding_Start();
    void Yetuga_Holding_End();


	void Save_Json(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);

	void Save_Json_Animation(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_uint m_iNumLevels = {};
	vector<class CCamera*> m_pCameras[ENUM_CLASS(LEVEL::END)];
	class CCamera* m_pActiveCamera = { nullptr };

public:
	static CCamera_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END