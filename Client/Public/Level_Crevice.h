#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Crevice final : public CLevel
{
private:
	CLevel_Crevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Crevice() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CClientInstance* m_pClientInstance = { nullptr };

	vector<future<HRESULT>> m_futures;

	recursive_mutex m_RecurMutex;

	mutex m_Mutex;

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI();

	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

	HRESULT Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);


public:
	static CLevel_Crevice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END