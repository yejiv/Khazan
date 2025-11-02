#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_HeinMach final : public CLevel
{
private:
	CLevel_HeinMach(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_HeinMach() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Test(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapObject_Test(const _wstring& strLayerTag);

	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 서브 레벨 인덱스 | 4. 현재 로드할 레벨 | 5. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	// 서브 레벨 오브젝트
	HRESULT Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 현재 로드할 레벨 | 4. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	// 특정 서브 레벨 오브젝트
	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 상호 작용 오브젝트
	HRESULT Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 인스턴싱 오브젝트
	HRESULT Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

	// 파라미터 ( 1. 데이터 파일 이름 | 2. 현재 로드할 레벨 | 3. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	// 조명 불러오기
	HRESULT Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

private:
	class CClientInstance* m_pClientInstance = { nullptr };

	vector<future<HRESULT>> m_futures;

	recursive_mutex m_Mutex;

public:
	static CLevel_HeinMach* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END