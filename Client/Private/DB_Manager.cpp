#include "DB_Manager.h"
#include "GameInstance.h"
#include <codecvt>

CDB_Manager::CDB_Manager()
{
}

HRESULT CDB_Manager::Load_Data(DATATYPE eType, const _tchar* pFilePath)
{
	if (eType == DATATYPE::ITEM)
		Load_ItemDB(pFilePath);
	else if (eType == DATATYPE::EQUIPEFFECT)
		Load_Equip_EffectDB(pFilePath);
	else if (eType == DATATYPE::OTHEREFFECT)
		Load_Other_EffectDB(pFilePath);

	return S_OK;
}


_wstring CDB_Manager::Load_UTF8ToWString(const std::wstring& filePath)
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

_float CDB_Manager::Read_float(wstringstream& s)
{
	_wstring t;
	getline(s, t, L'\,');
	return t.empty() ? 0.f : stof(t);
}

_uint CDB_Manager::Read_UInt(wstringstream& s)
{
	_wstring t;
	getline(s, t, L'\,');
	return t.empty() ? 0 : stoul(t);
}

_wstring CDB_Manager::Read_WString(wstringstream& s)
{
	_wstring t;
	getline(s, t, L'\,');
	return t;
}

HRESULT CDB_Manager::Load_ItemDB(const _tchar* pFilePath)
{
	_wstring wText = Load_UTF8ToWString(pFilePath);

	if (wText.empty())
	{
		MSG_BOX(TEXT("CSV 파일 로드 실패"));
		return E_FAIL;
	}

	wstringstream fileStream(wText);
	_wstring line = {};
	_bool bHeader = true;

    while (getline(fileStream, line))
    {
        if (bHeader)
        {
            bHeader = false;
            continue;
        }

        wstringstream ss(line);
        ITEM_DATA data{};
		_int ID = data.iID = Read_UInt(ss);
		data.strName = Read_WString(ss);
		data.iType = Read_UInt(ss);
		data.iEffect_ID = Read_UInt(ss);
		data.iLevel = Read_UInt(ss);
		data.iGrade = Read_UInt(ss);
		_int isBool = Read_UInt(ss);
		data.bisDrop = (isBool != 0);
		
		isBool = Read_UInt(ss);
		data.bisBox = (isBool != 0);

		data.iGold = Read_UInt(ss);
		data.iLachryma = Read_UInt(ss);
		data.iMaxValue = Read_UInt(ss);
		data.iTexPass = Read_UInt(ss);

		data.strIconName = Read_WString(ss);
		data.strText = Read_WString(ss);

		m_ItemData.emplace(ID, data);
    }

	return S_OK;
}

HRESULT CDB_Manager::Load_Equip_EffectDB(const _tchar* pFilePath)
{
	_wstring wText = Load_UTF8ToWString(pFilePath);

	if (wText.empty())
	{
		MSG_BOX(TEXT("CSV 파일 로드 실패"));
		return E_FAIL;
	}

	wstringstream fileStream(wText);
	_wstring line = {};
	_bool bHeader = true;

	while (getline(fileStream, line))
	{
		if (bHeader)
		{
			bHeader = false;
			continue;
		}

		wstringstream ss(line);
		EQUIPITEM_DATA data{};

		_int ID = data.iID = Read_UInt(ss);
		data.strName = Read_WString(ss);
		data.iType = Read_UInt(ss);
		data.iValue_Type_1 = Read_UInt(ss);
		data.iValue_1 = Read_UInt(ss);
		data.iValue_Type_2 = Read_UInt(ss);
		data.iValue_2 = Read_UInt(ss);
		data.iValue_Type_3 = Read_UInt(ss);
		data.iValue_3 = Read_UInt(ss);
		data.iClother_Type = Read_UInt(ss);
		data.fWeight = Read_float(ss);

		m_EquipEffectData.emplace(ID, data);
	}

	return S_OK;
}

HRESULT CDB_Manager::Load_Other_EffectDB(const _tchar* pFilePath)
{
	_wstring wText = Load_UTF8ToWString(pFilePath);

	if (wText.empty())
	{
		MSG_BOX(TEXT("CSV 파일 로드 실패"));
		return E_FAIL;
	}

	wstringstream fileStream(wText);
	_wstring line = {};
	_bool bHeader = true;

	while (getline(fileStream, line))
	{
		if (bHeader)
		{
			bHeader = false;
			continue;
		}

		wstringstream ss(line);
		OTHERITEM_DATA data{};

		_int ID =  data.iID = Read_UInt(ss);
		data.strName = Read_WString(ss);
		data.iState_Type_1 = Read_UInt(ss);
		data.iValue_Type_1 = Read_UInt(ss);
		data.iValue_1 = Read_UInt(ss);
		data.iState_Type_2 = Read_UInt(ss);
		data.iValue_Type_2 = Read_UInt(ss);
		data.iValue_2 = Read_UInt(ss);
		data.iState_Type_3 = Read_UInt(ss);
		data.iValue_Type_3 = Read_UInt(ss);
		data.iValue_3 = Read_UInt(ss);
		data.strText = Read_WString(ss);

		m_OtherEffectData.emplace(ID, data);
	}

	return S_OK;
}

CDB_Manager* CDB_Manager::Create()
{
	return new CDB_Manager();
}

void CDB_Manager::Free()
{
	__super::Free();

}
