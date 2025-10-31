#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CDB_Manager final : public CBase
{
private:
	CDB_Manager();
	virtual ~CDB_Manager() = default;

public:
	HRESULT									Load_Data(DATATYPE eType, const _tchar* pFilePath);
	
	template <typename T>
	const T*								Get_Data(_uint iID) const;

private:
	unordered_map<_uint, ITEM_DATA>			m_ItemData = {};
	unordered_map<_uint, EQUIPITEM_DATA>	m_EquipEffectData = {};
	unordered_map<_uint, OTHERITEM_DATA>	m_OtherEffectData = {};
	unordered_map<_uint, STATE_DATA>		m_StateData = {};

private:
	_wstring								Load_UTF8ToWString(const wstring& filePath);	//UTF8 변환
	_float									Read_float(wstringstream& s);
	_uint									Read_UInt(wstringstream& s);					//인트 값 읽기
	_wstring								Read_WString(wstringstream& s);					//스트링 값 읽기

	HRESULT									Load_ItemDB(const _tchar* pFilePath);
	HRESULT									Load_Equip_EffectDB(const _tchar* pFilePath);
	HRESULT									Load_Other_EffectDB(const _tchar* pFilePath);
	HRESULT									Load_StateDB(const _tchar* pFilePath);


public:
	static CDB_Manager*						Create();
	virtual void							Free() override;

};

template<typename T>
inline const T* CDB_Manager::Get_Data(_uint iID) const
{
	MSG_BOX(TEXT("DB : 없는 데이터 타입 접근"));
	return nullptr;
}

template <>
inline const ITEM_DATA* CDB_Manager::Get_Data<ITEM_DATA>(_uint iID) const
{
	auto Data = m_ItemData.find(iID);
	return (Data != m_ItemData.end()) ? &Data->second : nullptr;
}

template <>
inline const EQUIPITEM_DATA* CDB_Manager::Get_Data<EQUIPITEM_DATA>(_uint iID) const
{
	auto Data = m_EquipEffectData.find(iID);
	return (Data != m_EquipEffectData.end()) ? &Data->second : nullptr;
}

template <>
inline const OTHERITEM_DATA* CDB_Manager::Get_Data<OTHERITEM_DATA>(_uint iID) const
{
	auto Data = m_OtherEffectData.find(iID);
	return (Data != m_OtherEffectData.end()) ? &Data->second : nullptr;
}

template <>
inline const STATE_DATA* CDB_Manager::Get_Data<STATE_DATA>(_uint iID) const
{
	auto Data = m_StateData.find(iID);
	return (Data != m_StateData.end()) ? &Data->second : nullptr;
}


NS_END