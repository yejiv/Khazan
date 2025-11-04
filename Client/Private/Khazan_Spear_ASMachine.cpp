#include "Khazan_Spear_ASMachine.h"
#include "GameInstance.h"
#include <codecvt>

const CKhazan_Spear_ASMachine::SPEAR_PRIORTIY  CKhazan_Spear_ASMachine::s_CategoryPriorities[] =
{
	/*   이름				카테고리		서브타입			우선순위	*/					
	{TEXT("DIE"),			(1 << 0),		   0,		5},
											   
	{TEXT("HOLD"),			(1 << 1),		   0,		15},
	{TEXT("GROGGY"),		(1 << 2),		   0,		15},
	{TEXT("DAMAGED"),		(1 << 3),		   0,		15},

	{TEXT(" SKILL     "),	(1 << 4),		   0,		25},

	{TEXT("GUARD"),			(1 << 5),		   0,		35},

	{TEXT("ATTACK	  "),	(1 << 6),		   0,		45},
	{TEXT(" FALL      "),	(1 << 6),	(1 << 0),		45},
	{TEXT(" FAST      "),	(1 << 6),	(1 << 1),		45},
	{TEXT(" GRAPPLE   "),	(1 << 6),	(1 << 2),		45},
	//{TEXT(" SKILL     "),	(1 << 6),	(1 << 3),		45},
	{TEXT(" COUNTER   "),	(1 << 6),	(1 << 4),		45},
	{TEXT(" DODGEATK  "),	(1 << 6),	(1 << 5),		45},
	{TEXT(" REFLECTION"),	(1 << 6),	(1 << 6),		45},
	{TEXT(" SPRINTATK "),	(1 << 6),	(1 << 7),		45},
	{TEXT(" STRONG    "),	(1 << 6),	(1 << 8),		45},
	{TEXT(" JAVELIN   "),	(1 << 6),	(1 << 9),		45},
	{TEXT(" CHARGE    "),	(1 << 6),	(1 << 10),		45},

	{TEXT(" MOVE	   "),  (1 << 7),	       0,		45},
	{TEXT(" IDLE       "),	(1 << 7),	(1 << 0),		1000},// 예외
	{TEXT(" WALK       "),	(1 << 7),	(1 << 1),		45},
	{TEXT(" RUN        "),	(1 << 7),	(1 << 2),		45},
	{TEXT(" SPRINT     "),	(1 << 7),	(1 << 3),		45},
	{TEXT(" CLIMB      "),	(1 << 7),	(1 << 4),		45}, //
	{TEXT(" MIRAGE_STEP"),	(1 << 7),	(1 << 5),		44}, //
	{TEXT(" GETUP      "),	(1 << 7),	(1 << 6),		45},
	{TEXT(" FALL       "),	(1 << 7),	(1 << 7),		45},
	{TEXT(" DODGE      "),	(1 << 7),	(1 << 8),		44}, //




	{TEXT("LOCKON"),		(1 << 8), 0, 55},
									  
	{TEXT("INTERACT"),		(1 << 9), 0, 65},
	{TEXT("WEAPON_CHANGE"), (1 << 10), 0, 65},

	//{TEXT(" IDLE       "),	(1 << 11),	0,		1000},
};

CKhazan_Spear_ASMachine::CKhazan_Spear_ASMachine()
{
}

HRESULT CKhazan_Spear_ASMachine::Initialize_Prototype()
{
	if (FAILED(Data_Load()))
		return E_FAIL;

	return S_OK;

}

_int CKhazan_Spear_ASMachine::Get_AnimIndexByName(const wstring& strName) const
{
	auto it = m_AnimNameToIndex.find(strName);
	if (it != m_AnimNameToIndex.end())
		return it->second;

	return -1;  // 못 찾음
}

vector<_uint> CKhazan_Spear_ASMachine::Get_AnimIndicesByCategory(_uint iCategory) const
{
	vector<_uint> result;
	for (_uint i = 0; i < m_ASMs.size(); ++i)
	{
		if (m_ASMs[i].iCategory & iCategory)
			result.push_back(i);
	}
	return result;
}

vector<_uint> CKhazan_Spear_ASMachine::Get_AnimIndicesByAttack(_uint iAttack) const
{
	vector<_uint> result;
	for (_uint i = 0; i < m_ASMs.size(); ++i)
	{
		if (m_ASMs[i].iAttack & iAttack)
			result.push_back(i);
	}
	return result;
}

vector<_uint> CKhazan_Spear_ASMachine::Get_AnimIndicesByMove(_uint iMove) const
{
	vector<_uint> result;
	for (_uint i = 0; i < m_ASMs.size(); ++i)
	{
		if (m_ASMs[i].iMove & iMove)
			result.push_back(i);
	}
	return result;
}

_int CKhazan_Spear_ASMachine::Get_AnimIndex(_uint iCategory, _uint iSubType) const
{
	for (_uint i = 0; i < m_ASMs.size(); ++i)
	{

		if (!(m_ASMs[i].iCategory & iCategory))
			continue;

		if (iCategory & CATEGORY::M_MOVE)
		{
			if (m_ASMs[i].iMove & iSubType)
				return i;
		}
		else if (iCategory & CATEGORY::M_ATTACK)
		{
			if (m_ASMs[i].iAttack & iSubType)
				return i;
		}
	}

	return -1;
}

_uint CKhazan_Spear_ASMachine::Get_CategoryPriority(_uint iCategory, _uint iSubType) const
{
	for (const auto& cp : s_CategoryPriorities)
	{
		if (cp.iCategory == iCategory && cp.iSubType == iSubType)
			return cp.iPriority;
	}
	return 99;  // 알 수 없는 카테고리는 최하위 우선순위
}

// 우선 순위만 알면 되므로 sub까지만.
_bool CKhazan_Spear_ASMachine::Can_Interrupt(_uint iCurCategory, _uint iCurSubType, _uint iNewCategory, _uint iNewSubType) const
{
	_uint iCurrentPriority = Get_CategoryPriority(iCurCategory, iCurSubType);
	_uint iNewPriority = Get_CategoryPriority(iNewCategory, iNewSubType);

	// 우선순위가 높을수록 낮은 숫자
	// 새 카테고리의 우선순위가 더 높으면 (숫자가 작으면) 중단 가능!!
	return iNewPriority < iCurrentPriority;
}

_bool CKhazan_Spear_ASMachine::Check_CoolTime(_uint iAnimationIndex)
{
	return m_CoolTimes[iAnimationIndex].isEnble;
}

void CKhazan_Spear_ASMachine::Update_CoolTime(_float fTimeDelta, _uint iAnimationIndex)
{
	if (m_CoolTimes[iAnimationIndex].fMaxCoolTime > 0.f && !m_CoolTimes[iAnimationIndex].isEnble)
		m_CoolTimes[iAnimationIndex].isEnble = true;

	for (auto cool : m_CoolTimes)
	{
		if (cool.isEnble)
		{
			cool.fCurCooltime += fTimeDelta;
			
			if (cool.fCurCooltime >= cool.fMaxCoolTime)
			{	
				cool.fCurCooltime = 0.f;
				cool.isEnble = false;
			}
		}
	}
}


HRESULT CKhazan_Spear_ASMachine::Data_Load()
{

	_wstring wText = Load_UTF8ToWString(TEXT("../Bin/Data/DB/Khazan_Spear_Animation_DB.csv"));

	if (wText.empty())
	{
		MSG_BOX(TEXT(" [CKhazan_Spear_ASMachine] CSV 파일 로드 실패"));
		return E_FAIL;
	}

	wstringstream fileStream(wText);
	_wstring line = {};
	_bool bHeader = true;

	auto read_wstring = [&](wstringstream& s) {
		_wstring t;
		getline(s, t, L'\,');
		t.erase(0, t.find_first_not_of(L" \t\r\n"));
		t.erase(t.find_last_not_of(L" \t\r\n") + 1);
		return t;
		};

	auto read_uint = [&](wstringstream& s) {
		_wstring t;
		getline(s, t, L'\,');
		t.erase(0, t.find_first_not_of(L" \t\r\n"));
		t.erase(t.find_last_not_of(L" \t\r\n") + 1);
		return t.empty() ? 0 : stoul(t);
		};

	_uint iAnimIndex = { 0 };
	while (getline(fileStream, line))
	{
		if (bHeader)
		{
			bHeader = false;
			continue;
		}


		// 빈 줄 스킵
		if (line.empty() || line.find_first_not_of(L" \t\r\n") == wstring::npos)
			continue;

		wstringstream ss(line);
		SPEAR_ASM asmData = {};
		_uint iID = read_uint(ss);	// Index (사용 안 함)
		_wstring strName = read_wstring(ss);			// name

		// weapon
		_wstring strWeapon = read_wstring(ss);			
		asmData.iWeapon = Parse_Weapon(strWeapon);

		//CATEGORY == state
		_wstring strState0 = read_wstring(ss);
		_wstring strState1 = read_wstring(ss);
		asmData.iCategory = 0;
		if (!strState0.empty() && strState0 != L"0") asmData.iCategory |= Parse_Category(strState0);
		if (!strState1.empty() && strState1 != L"0") asmData.iCategory |= Parse_Category(strState1);

		//SET 
		asmData.iSet = read_uint(ss);

		//CYCLE
		_wstring strCycle = read_wstring(ss);
		if (!strCycle.empty() && strCycle != L"0") asmData.iCycle |= Parse_Cycle(strCycle);

		// ATTACK
		_wstring strAttack0 = read_wstring(ss);
		_wstring strAttack1 = read_wstring(ss);
		asmData.iAttack = 0;
		if (!strAttack0.empty() && strAttack0 != L"0") asmData.iAttack |= Parse_Attack(strAttack0);
		if (!strAttack1.empty() && strAttack1 != L"0") asmData.iAttack |= Parse_Attack(strAttack1);

		// SKILL
		_wstring strSkill = read_wstring(ss);
		if (!strSkill.empty() && strSkill != L"0") asmData.iSkill = (_uint)_wtoi(strSkill.c_str());
		else	asmData.iSkill = 0;

		// move
		_wstring strMove = read_wstring(ss);
		asmData.iMove = 0;
		if (!strMove.empty() && strMove != L"0") asmData.iMove = Parse_Move(strMove);

		// move sub
		_wstring strMoveSub = read_wstring(ss);
		asmData.iMoveSub = 0;
		if (!strMoveSub.empty() && strMoveSub != L"0") asmData.iMoveSub = Parse_MoveSub(strMoveSub);


		// direction
		_wstring strDirection = read_wstring(ss);
		asmData.iDirection = 0;
		if (!strMove.empty() && strMove != L"0") asmData.iMove = Parse_Direction(strMove);

		// guard
		read_wstring(ss);

		// groggy 
		read_wstring(ss);

		// interact
		read_wstring(ss);

		// weaponchange 
		read_wstring(ss);

		// hold 
		read_wstring(ss);

		//damaged 
		read_wstring(ss);

		m_ASMs.push_back(asmData);

		m_AnimNameToIndex[strName] = iAnimIndex++;
	}

	return S_OK;

}

_wstring CKhazan_Spear_ASMachine::Load_UTF8ToWString(const std::wstring& filePath)
{
	ifstream file(filePath, ios::binary);
	if (!file.is_open())
		return L"";

	vector<_char> buffer((istreambuf_iterator<_char>(file)), {});
	file.close();

	if (buffer.empty())
		return L"";

	_int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), (_int)buffer.size(), nullptr, 0);
	std::wstring wstr(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, buffer.data(), (_int)buffer.size(), &wstr[0], sizeNeeded);
	return wstr;
}

_uint CKhazan_Spear_ASMachine::Parse_Weapon(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	if (lower == L"spear")return WEAPON::SPEAR;
	else if (lower == L"hand")return WEAPON::BAREHAND;
	else if (lower == L"none")return WEAPON::NONE;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_Category(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	if (lower == L"move") return CATEGORY::M_MOVE;
	else if (lower == L"attack") return CATEGORY::M_ATTACK;
	else if (lower == L"interact") return CATEGORY::M_INTERACT;
	else if (lower == L"damaged") return CATEGORY::M_DAMAGED;
	else if (lower == L"hold") return CATEGORY::M_HOLD;
	else if (lower == L"die") return CATEGORY::M_DIE;
	else if (lower == L"lockon") return CATEGORY::M_LOCKON;
	else if (lower == L"guard") return CATEGORY::M_GUARD;
	else if (lower == L"groggy") return CATEGORY::M_GROGGY;
	else if (lower == L"weaponchange") return CATEGORY::M_WEAPON_CHANGE;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_Cycle(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	if (lower == L"start") return CYCLE::CYCLE_START;
	else if (lower == L"loop") return CYCLE::CYCLE_LOOP;
	else if (lower == L"end") return CYCLE::CYCLE_END;
	else if (lower == L"endstart") return CYCLE::CYCLE_ENDSTART;
	else if (lower == L"endend") return CYCLE::CYCLE_ENDEND;
	//else if (lower == L"startstart") return CYCLE::CYCLE_STARTSTART;
	//else if (lower == L"startend") return CYCLE::CYCLE_STARTEND;
	else if (lower == L"_break") return CYCLE::CYCLE_BREAK;
	else if (lower == L"shot") return CYCLE::CYCLE_SHOT;
	else if (lower == L"fail") return CYCLE::CYCLE_FAIL;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_Attack(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	if (lower == L"fall") return ATTACK::ATK_FALL;
	else if (lower == L"fast") return ATTACK::ATK_FAST;
	else if (lower == L"grapple") return ATTACK::ATK_GRAPPLE;
	else if (lower == L"skill") return ATTACK::ATK_SKILL;
	else if (lower == L"counter") return ATTACK::ATK_COUNTER;
	else if (lower == L"dodge")	return ATTACK::ATK_DODGEATK;
	else if (lower == L"reflection") return ATTACK::ATK_REFLECTION;
	else if (lower == L"sprint") return ATTACK::ATK_SPRINTATK;
	else if (lower == L"strong") return ATTACK::ATK_STRONG;
	else if (lower == L"javelin") return ATTACK::ATK_JAVELIN;
	else if (lower == L"charge") return ATTACK::ATK_CHARGE;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_Skill(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	return (_uint)_wtoi(str.c_str());
}

_uint CKhazan_Spear_ASMachine::Parse_Move(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	//if (lower == L"idle") return MOVE::MOVE_IDLE;
	 if (lower == L"walk") return MOVE::MOVE_WALK;
	else if (lower == L"run") return MOVE::MOVE_RUN;
	else if (lower == L"sprint") return MOVE::MOVE_SPRINT;
	else if (lower == L"climb") return MOVE::MOVE_CLIMB;
	else if (lower == L"mirage") return MOVE::MOVE_MIRAGE_STEP;
	else if (lower == L"getup") return MOVE::MOVE_GETUP;
	else if (lower == L"fall") return MOVE::MOVE_FALL;
	else if (lower == L"dodge") return MOVE::MOVE_DODGE;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_MoveSub(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	if (lower == L"standturn") return MOVESUB::MOVESUB_STAND_TURN;
	else if (lower == L"stand") return MOVESUB::MOVESUB_STAND;
	else if (lower == L"standhard") return MOVESUB::MOVESUB_STAND_HARD;
	else if (lower == L"stand1") return MOVESUB::MOVESUB_STAND1;
	else if (lower == L"stand2") return MOVESUB::MOVESUB_STAND2;

	return 0;
}

_uint CKhazan_Spear_ASMachine::Parse_Direction(const wstring& str)
{
	if (str.empty() || str == L"0")
		return 0;

	wstring lower = str;
	transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

	_uint iDirection = 0;

	// 단일 방향 체크 (L180, R180)
	if (lower == L"l180")
		return static_cast<_uint>(DIRECTION::L180);
	if (lower == L"r180")
		return static_cast<_uint>(DIRECTION::R180);

	// cc는 무조건 앞에 있으므로 먼저 체크
	if (lower.find(L"cc") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::CC);
		// "cc" 제거 (이미 처리했으므로)
		size_t pos = lower.find(L"cc");
		lower.erase(pos, 2);
	}

	// c 체크 (cc가 아닌 경우)
	if (lower.find(L"c") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::C);
		size_t pos = lower.find(L"c");
		lower.erase(pos, 1);
	}

	// br
	if (lower.find(L"br") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::B);
		iDirection |= static_cast<_uint>(DIRECTION::R);
		size_t pos = lower.find(L"br");
		lower.erase(pos, 2);
	}

	// bl 
	if (lower.find(L"bl") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::B);
		iDirection |= static_cast<_uint>(DIRECTION::L);
		size_t pos = lower.find(L"bl");
		lower.erase(pos, 2);
	}

	// fr 
	if (lower.find(L"fr") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::F);
		iDirection |= static_cast<_uint>(DIRECTION::R);
		size_t pos = lower.find(L"fr");
		lower.erase(pos, 2);
	}

	// fl
	if (lower.find(L"fl") != wstring::npos)
	{
		iDirection |= static_cast<_uint>(DIRECTION::F);
		iDirection |= static_cast<_uint>(DIRECTION::L);
		size_t pos = lower.find(L"fl");
		lower.erase(pos, 2);
	}

	// 개별 방향
	if (lower.find(L"f") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::F);

	if (lower.find(L"b") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::B);

	if (lower.find(L"l") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::L);

	if (lower.find(L"r") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::R);

	if (lower.find(L"u") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::U);

	if (lower.find(L"d") != wstring::npos)
		iDirection |= static_cast<_uint>(DIRECTION::D);

	return iDirection;
}

CKhazan_Spear_ASMachine* CKhazan_Spear_ASMachine::Create()
{
	CKhazan_Spear_ASMachine* pInstance = new CKhazan_Spear_ASMachine();


	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKhazan_Spear_ASMachine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKhazan_Spear_ASMachine::Free()
{
	__super::Free();
}
