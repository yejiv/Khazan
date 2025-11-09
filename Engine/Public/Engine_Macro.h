#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{


	#ifndef			MSG_BOX
	#define			MSG_BOX(_message)			MessageBox(NULL, _message, L"System Message", MB_OK)
	#endif

	#define			ENUM_CLASS(ENUM)				static_cast<unsigned int>(ENUM)

	#define			NS_BEGIN(NAMESPACE)		namespace NAMESPACE {
	#define			NS_END						}
	
	#define			USING(NAMESPACE)	using namespace NAMESPACE;
	
	#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL		_declspec(dllexport)
	#else
	#define ENGINE_DLL		_declspec(dllimport)
	#endif

#pragma region CHECK_MACRO

#pragma region CHECK_CONDITION

	// true 면 return : void 반환 시 - (_condition, 비우기)
#define CHECK_TRUE(_condition, _return)	\
			do { if (true == _condition) { return _return; } } while (0)

	// true 면 return : void 반환 시 - (_condition, _message, 비우기)
#define CHECK_TRUE_MSG(_condition, _message, _return)	\
			do { if (true == _condition) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// true 면 return : void 반환 시 - (_condition, 비우기)
#define CHECK_TRUE_ASSERT(_condition, _return)	\
			do { if (true == _condition) { assert(false); return _return; } } while (0)

	// false 면 return : void 반환 시 - (_condition, 비우기)
#define CHECK_FALSE(_condition, _return)	\
			do { if (false == _condition) { return _return; } } while (0)

	// false 면 return : void 반환 시 - (_condition, _message, 비우기)
#define CHECK_FALSE_MSG(_condition, _message, _return)	\
			do { if (false == _condition) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// false 면 return : void 반환 시 - (_condition, 비우기)
#define CHECK_FALSE_ASSERT(_condition, _return)	\
			do { if (true == _condition) { assert(false); return _return; } } while (0)

#pragma endregion

#pragma region CHECK_EQUAL

	// _left == _right 면 return : void 반환 시 - (_left, _right, 비우기)
#define CHECK_EQUAL(_left, _right, _return)	\
			do { if (_left == _right) { return _return; } } while (0)

	// _left == _right 면 return : void 반환 시 - (_left, _right, _message, 비우기)
#define CHECK_EQUAL_MSG(_left, _right, _message, _return)	\
			do { if (_left == _right) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// _left == _right 면 return : void 반환 시 - (_left, _right, 비우기)
#define CHECK_EQUAL_ASSERT(_left, _right, _return)	\
			do { if (_left == _right) { assert(false); return _return; } } while (0)

	// _left != _right 면 return : void 반환 시 - (_left, _right, 비우기)
#define CHECK_NOT_EQUAL(_left, _right, _return)	\
			do { if (_left != _right) { return _return; } } while (0)

	// _left != _right 면 return : void 반환 시 - (_left, _right, _message, 비우기)
#define CHECK_NOT_EQUAL_MSG(_left, _right, _message, _return)	\
			do { if (_left != _right) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// _left != _right 면 return : void 반환 시 - (_left, _right, 비우기)
#define CHECK_NOT_EQUAL_ASSERT(_left, _right, _return)	\
			do { if (_left != _right) { assert(false); return _return; } } while (0)

#pragma endregion

#pragma region CHECK_NULLPTR

	// nullptr 이면 return : void 반환 시 - (_ptr, 비우기)
#define CHECK_NULLPTR(_ptr, _return)	\
			do { if (nullptr == _ptr) { return _return; } } while (0)

	// nullptr 이면 return : void 반환 시 - (_ptr, _message, 비우기)
#define CHECK_NULLPTR_MSG(_ptr, _message, _return)	\
			do { if (nullptr == _ptr) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// nullptr 이면 return : void 반환 시 - (_ptr, 비우기)
#define CHECK_NULLPTR_ASSERT(_ptr, _return)	\
			do { if (nullptr == _ptr) { assert(false); return _return; } } while (0)

	// nullptr 이 아니면 return : void 반환 시 - (_ptr, 비우기)
#define CHECK_NOT_NULLPTR(_ptr, _return)	\
			do { if (nullptr != _ptr) { return _return; } } while (0)

	// nullptr 이 아니면 return : void 반환 시 - (_ptr, _message, 비우기)
#define CHECK_NOT_NULLPTR_MSG(_ptr, _message, _return)	\
			do { if (nullptr != _ptr) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// nullptr 이 아니면 return : void 반환 시 - (_ptr, 비우기)
#define CHECK_NOT_NULLPTR_ASSERT(_ptr, _return)	\
			do { if (nullptr != _ptr) { assert(false); return _return; } } while (0)

#pragma endregion

#pragma region CHECK_FAILED

	// FAILED 떨어지면 return : void 반환 시 - (_hr, 비우기)
#define CHECK_FAILED(_hr, _return)	\
			do { if (FAILED(_hr)) { return _return; } } while (0)

	// FAILED 떨어지면 return : void 반환 시 - (_hr, _message, 비우기)
#define CHECK_FAILED_MSG(_hr, _message, _return)	\
			do { if (FAILED(_hr)) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// FAILED 떨어지면 return : void 반환 시 - (_hr, 비우기)
#define CHECK_FAILED_ASSERT(_hr, _return)	\
			do { if (FAILED(_hr)) { assert(false); return _return; } } while (0)

	// SUCCEEDED 떨어지면 return : void 반환 시 - (_hr, 비우기)
#define CHECK_SUCCEEDED(_hr, _return)	\
			do { if (SUCCEEDED(_hr)) { return _return; } } while (0)

	// SUCCEEDED 떨어지면 return : void 반환 시 - (_hr, _message, 비우기)
#define CHECK_SUCCEEDED_MSG(_hr, _message, _return)	\
			do { if (SUCCEEDED(_hr)) { MessageBox(NULL, _message, L"System Message", MB_OK); return _return; } } while (0)

	// SUCCEEDED 떨어지면 return : void 반환 시 - (_hr, 비우기)
#define CHECK_SUCCEEDED_ASSERT(_hr, _return)	\
			do { if (SUCCEEDED(_hr)) { assert(false); return _return; } } while (0)

#pragma endregion

#pragma endregion

	#define NULL_CHECK( _ptr)	\
		{if( _ptr == 0){ return;}}
	
	#define NULL_CHECK_RETURN( _ptr, _return)	\
		{if( _ptr == 0){return _return;}}
	
	#define NULL_CHECK_MSG( _ptr, _message )		\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);}}
	
	#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);return _return;}}
	
	#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return E_FAIL;}
	
	#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return _return;}
	
	#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return E_FAIL;}
	
	#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return _return;}
	
	#define NUM_FRAMES_IN_FLIGHT 3
	#define MAX_FILENAME_SIZE 100
	#define U8STR(str) reinterpret_cast<const char*>(u8##str)
	
	#define NO_COPY(CLASSNAME)										\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		
	
	#define DECLARE_SINGLETON(CLASSNAME)							\
			NO_COPY(CLASSNAME)										\
			private:												\
			static CLASSNAME*	m_pInstance;						\
			public:													\
			static CLASSNAME*	GetInstance( void );				\
			static void DestroyInstance( void );			
	
	#define IMPLEMENT_SINGLETON(CLASSNAME)							\
			CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
				if(nullptr == m_pInstance) {						\
					m_pInstance = new CLASSNAME;					\
				}													\
				return m_pInstance;									\
			}														\
			void CLASSNAME::DestroyInstance( void ) {				\
				if(nullptr != m_pInstance)	{						\
					delete m_pInstance;								\
					m_pInstance = nullptr;							\
				}													\
			}


	#define		TEXTURETYPE_NONE						0
	#define		TEXTURETYPE_DIFFUSE						1
	#define		TEXTURETYPE_SPECULAR					2
	#define		TEXTURETYPE_AMBIENT 					3
	#define		TEXTURETYPE_EMISSIVE 					4
	#define		TEXTURETYPE_HEIGHT 						5
	#define		TEXTURETYPE_NORMALS 					6
	#define		TEXTURETYPE_SHININESS 					7
	#define		TEXTURETYPE_OPACITY 					8
	#define		TEXTURETYPE_DISPLACEMENT 				9
	#define		TEXTURETYPE_LIGHTMAP					10
	#define		TEXTURETYPE_REFLECTION 					11
	#define		TEXTURETYPE_BASE_COLOR 					12
	#define		TEXTURETYPE_NORMAL_CAMERA 				13
	#define		TEXTURETYPE_EMISSION_COLOR 				14
	#define		TEXTURETYPE_METALNESS 					15
	#define		TEXTURETYPE_DIFFUSE_ROUGHNESS 			16
	#define		TEXTURETYPE_AMBIENT_OCCLUSION 			17
	#define		TEXTURETYPE_UNKNOWN 					18
	#define		TEXTURETYPE_SHEEN 						19
	#define		TEXTURETYPE_CLEARCOAT 					20
	#define		TEXTURETYPE_TRANSMISSION 				21
	#define		TEXTURETYPE_MAYA_BASE 					22
	#define		TEXTURETYPE_MAYA_SPECULAR 				23
	#define		TEXTURETYPE_MAYA_SPECULAR_COLOR 		24
	#define		TEXTURETYPE_MAYA_SPECULAR_ROUGHNESS 	25
	#define		TEXTURETYPE_ANISOTROPY 					26
	#define		TEXTURETYPE_GLTF_METALLIC_ROUGHNESS 	27
	#define		TEXTURETYPE_MAX							27
}

#endif // Engine_Macro_h__
