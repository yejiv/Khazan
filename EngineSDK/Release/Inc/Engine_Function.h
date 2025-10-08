#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();
		return dwRefCnt;
	}





	inline string WStringToAnsi(const std::wstring& w, UINT codepage = CP_ACP)
	{
		if (w.empty()) return {};
		const int size = WideCharToMultiByte(codepage, 0,
			w.c_str(), (int)w.size(),
			nullptr, 0, nullptr, nullptr);
		string out(size, '\0');
		WideCharToMultiByte(codepage, 0,
			w.c_str(), (int)w.size(),
			out.data(), size, nullptr, nullptr);
		return out;
	}

}

#endif // Engine_Function_h__
