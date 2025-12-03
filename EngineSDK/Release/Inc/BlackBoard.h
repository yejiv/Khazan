#pragma once
#include "Base.h"
#include "variant"

NS_BEGIN(Engine)

// Black Board에서 사용할 수 있는 타입을 ENUM으로 미리 정의
enum class BBKEYTYPE { BOOL, FLOAT, UINT, VECTOR3, OBJECT, COMPONENT };

class ENGINE_DLL CBlackBoard final : public CBase
{
public:
	// variant : 컴파일 과정 중에 사용할 변수들을 등록할 수 있게 해준다.
	typedef variant<_bool, _float, _uint, _float3, class CGameObject*, class CComponent*> BBVALUE;


protected:
	CBlackBoard();
	virtual ~CBlackBoard() = default;

public:
	template<typename T>
	void Set_Value(const string& Tag, const string& Key, const T& Value)
	{
		// 해당 테그의 해당 키 값에 값을 저장시킨다.
		m_Data[Tag][Key] = Value;
		m_KeyTypes[Tag][Key] = CheckType<T>();
	}

	template<typename T>
	T Get_Value(const string& Tag, const string& Key) const
	{
		// 몬스터 테그를 먼저 검색하고
		auto tagIter = m_Data.find(Tag);
		if (tagIter == m_Data.end())
			return T{};

		// 해당 테그로 접근된 키값의 Value를 찾는다.
		const auto& Data = tagIter->second;
		auto keyIter = Data.find(Key);
		if (keyIter == Data.end())
			return T{};

		// get : variant 에서 해당 값을 가져오는 함수
		return get<T>(keyIter->second);
	}


protected:
	// 타입 추론용 구현부를 템플릿 특수화로 나눠준다.
	template<typename T> static BBKEYTYPE CheckType();

protected:
	// string은 블랙보드를 각각의 몬스터 타입별로 나누기 위해서 추가하였다.
	unordered_map<string, unordered_map<string, BBVALUE>>	m_Data;
	unordered_map<string, unordered_map<string, BBKEYTYPE>>	m_KeyTypes;


public:
	static CBlackBoard*		Create();
	virtual void			Free();

};

// 템플릿 특수화 : 특정 타입일때만 이함수를 구현해라 
template<> inline BBKEYTYPE CBlackBoard::CheckType<_bool>() { return BBKEYTYPE::BOOL; }
template<> inline BBKEYTYPE CBlackBoard::CheckType<_float>() { return BBKEYTYPE::FLOAT; }
template<> inline BBKEYTYPE CBlackBoard::CheckType<_uint>() { return BBKEYTYPE::UINT; }
template<> inline BBKEYTYPE CBlackBoard::CheckType<_float3>() { return BBKEYTYPE::VECTOR3; }
template<> inline BBKEYTYPE CBlackBoard::CheckType<class CGameObject*>() { return BBKEYTYPE::OBJECT; }
template<> inline BBKEYTYPE CBlackBoard::CheckType<class CComponent*>() { return BBKEYTYPE::COMPONENT; }

NS_END