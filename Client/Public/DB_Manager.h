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

	template <typename T>
	const T*								Get_DataTalbe() const;

    _bool Exist_ID(_uint iID);

private:

	unordered_map<_uint, ITEM_DATA>			m_ItemData = {};

	unordered_map<_uint, EQUIPITEM_DATA>	m_EquipEffectData = {};
	unordered_map<_uint, OTHERITEM_DATA>	m_OtherEffectData = {};
	unordered_map<_uint, STATE_DATA>		m_StateData = {};
	unordered_map<_uint, ANNOUNCE_TALK_DB>	m_Announce_Talk_Data = {};
	unordered_map<_uint, SKILL_DB>			m_Skill_Data = {};
    unordered_map<_uint, DANJINJAR_DB>	    m_Danginjar_Data = {};
    vector<_uint>                           m_ItemIndex;

	
private:
	_wstring								Load_UTF8ToWString(const wstring& filePath);	//UTF8 변환
	_float									Read_float(wstringstream& s);
	_uint									Read_UInt(wstringstream& s);					//인트 값 읽기
	_wstring								Read_WString(wstringstream& s);					//스트링 값 읽기

	HRESULT									Load_ItemDB(const _tchar* pFilePath);
	HRESULT									Load_Equip_EffectDB(const _tchar* pFilePath);
	HRESULT									Load_Other_EffectDB(const _tchar* pFilePath);
	HRESULT									Load_StateDB(const _tchar* pFilePath);
	HRESULT									Load_Announce_TalkDB(const _tchar* pFilePath);
	HRESULT									Load_Skill_DB(const _tchar* pFilePath);
    HRESULT									Load_Danginjar_DB(const _tchar* pFilePath);

public:
	static CDB_Manager*						Create();
	virtual void							Free() override;
};

//Data접근
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

template <>
inline const ANNOUNCE_TALK_DB* CDB_Manager::Get_Data<ANNOUNCE_TALK_DB>(_uint iID) const
{
	auto Data = m_Announce_Talk_Data.find(iID);
	return (Data != m_Announce_Talk_Data.end()) ? &Data->second : nullptr;
}

template <>
inline const SKILL_DB* CDB_Manager::Get_Data<SKILL_DB>(_uint iID) const
{
	auto Data = m_Skill_Data.find(iID);
	return (Data != m_Skill_Data.end()) ? &Data->second : nullptr;
}

template <>
inline const DANJINJAR_DB* CDB_Manager::Get_Data<DANJINJAR_DB>(_uint iID) const
{
    auto Data = m_Danginjar_Data.find(iID);
    return (Data != m_Danginjar_Data.end()) ? &Data->second : nullptr;
}

//Table접근
template<typename T>
inline const T* CDB_Manager::Get_DataTalbe() const
{
	MSG_BOX(TEXT("DB : 없는 데이터 타입 접근"));
	return nullptr;
}
using ITEM_TABLE = unordered_map<_uint, ITEM_DATA>;
template <>
inline const ITEM_TABLE* CDB_Manager::Get_DataTalbe<ITEM_TABLE>() const 
{
	return &m_ItemData;
}

using EQUIIPEFFECT_TABLE = unordered_map<_uint, EQUIPITEM_DATA>;
template <>
inline const EQUIIPEFFECT_TABLE* CDB_Manager::Get_DataTalbe<EQUIIPEFFECT_TABLE>() const
{
	return &m_EquipEffectData;
}

using OTHERITEM_TABLE = unordered_map<_uint, OTHERITEM_DATA>;
template <>
inline const OTHERITEM_TABLE* CDB_Manager::Get_DataTalbe<OTHERITEM_TABLE>() const
{
	return &m_OtherEffectData;
}

using STATE_TABLE = unordered_map<_uint, STATE_DATA>;
template <>
inline const STATE_TABLE* CDB_Manager::Get_DataTalbe<STATE_TABLE>() const
{
	return &m_StateData;
}

using ANNOUNCE_TABLE = unordered_map<_uint, ANNOUNCE_TALK_DB>;
template <>
inline const ANNOUNCE_TABLE* CDB_Manager::Get_DataTalbe<ANNOUNCE_TABLE>() const
{
	return &m_Announce_Talk_Data;
}

using SKILL_TABLE = unordered_map<_uint, SKILL_DB>;
template <>
inline const SKILL_TABLE* CDB_Manager::Get_DataTalbe<SKILL_TABLE>() const
{
	return &m_Skill_Data;
}
NS_END