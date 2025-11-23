#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 실제 내가 보여주고자 하는 레벨을 준비해주기위한 레벨 클래스 */
/* 로딩 -> 로고 */
/* 로딩 -> 게임플레이 */

/* 로고, 게임플레이 -> 객체, 리소스들, 사운드, 쉐이더 ->자원준비를 거치는 과정 => 로딩을 하기위한
로더객체를 생성해준다ㅣ. */

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	enum class LOADING_STATE {UI_ON, INPUT, NEXTLEVEL, END };
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	void			Complete();
public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL				m_eNextLevelID = { LEVEL::END };
	class CLoader*		m_pLoader = { nullptr };
	class CClientInstance* m_pClientInstance = { nullptr };

	LOADING_STATE		m_eLoadingState = { LOADING_STATE::END };

    _bool               m_isPlayerSound = { false };
private:
	HRESULT Ready_GameObjects();

	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

    HRESULT Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
    HRESULT Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

	HRESULT Ready_LoadingThread();

	HRESULT Ready_ObjectLayer();

    HRESULT Ready_CleanImgUI();

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END