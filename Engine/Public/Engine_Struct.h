#ifndef Engine_Struct_h__
#define Engine_Struct_h__



namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInst;
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;
		unsigned int	iNumLevels;
		unsigned int	iNumJoltObjectLayer;

		unsigned int	iWinSizeX_Imgui, iWinSizeY_Imgui;
		list<wstring>	Menu_Imgui;
	}ENGINE_DESC;

	typedef struct tagLightDesc
	{
		enum TYPE { DIRECTIONAL, POINT, END };

		TYPE		eType;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;
	}LIGHT_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;

		float		fTrackPosition;
	}KEYFRAME;

	typedef struct tagUIKeyFrame
	{
		XMFLOAT2	vTransloation;
		float		fSize;
		float		fAlpha;
		float		fAngle;
		string		szEvent;

		float		fTrackPosition;
	}UIKEYFRAME;

	typedef struct tagMapObjectProperties
	{
		bool isSnow{ false };
		bool isCollider{ false };
		bool isIce{ false };
		bool isInstance{ false };
		bool isShadow{ false };
		bool isBackGround{ false };

		tagMapObjectProperties() = default;

		operator bool() const noexcept
		{
			return isSnow || isCollider || isIce ||
				isInstance || isShadow || isBackGround;
		}

	}MAPOBJECT_PROPERTIES;

	typedef struct tagInteractiveDesc		// 공부하고 추가 예정
	{
		enum TYPE { SAVEPOINT, LADDER, LEVER, ELEVATOR, PLZADD, END };

		bool isOnce{ false };				// 한번만 작동하는지
		bool isActive{ false };				// 활성화 되었는지
		float fInteractiveRange{};			// 상호작용 범위
		unsigned int iTriggerID{};			// 어떤 트리거랑 연동할 지

	}INTERACTIVE_DESC;

	typedef struct tagCatmullrom
	{
		XMVECTOR v1;
		XMVECTOR v2;
		XMVECTOR v3;
		XMVECTOR v4;
	}CATMULLROM;

	typedef struct tagCameraKeyFrame
	{
		XMFLOAT3 vTranslation;
		XMFLOAT4 vLookAt;
		float fSpeed;

		float fTrackPosition;
	}CAMERA_KEYFRAME;

	typedef struct tagCameraEvent
	{
		wstring strEventKey;
		unsigned int iEventType;
		bool isComplete;

		float fTrackPosition;
	}CAMERA_EVENT_DATA;

	typedef struct tagCoInitGuard {
		HRESULT hr = S_OK;
		tagCoInitGuard(DWORD coinit = COINIT_MULTITHREADED) { hr = CoInitializeEx(nullptr, coinit); }
		~tagCoInitGuard() { if (SUCCEEDED(hr)) CoUninitialize(); }
	}CO_INIT_GUARD;

	typedef struct tagComputePassDesc		// 컴퓨트 셰이더용 데이터
	{
		vector<ID3D11ShaderResourceView*>	SRVs;					// 읽기 전용
		vector<ID3D11UnorderedAccessView*>	UAVs;					// 읽기 / 쓰기
		vector<unsigned int>				UAVInitialCounts;		// 각 UAV 초기 카운트(필요 시 사용)
		vector<ID3D11Buffer*>				ConstantBuffers;		// 상수 버퍼

		unsigned int						x = 1, y = 1, z = 1;	// 디스패치할 스레드 그룹 개수
	}COMPUTE_PASS_DESC;

	typedef struct tagCascadeData
	{
		unsigned int			iNumCascades;
	
		vector<float>			Splits;
		vector<XMFLOAT4X4>		LightViewMatrices;
		vector<XMFLOAT4X4>		LightProjMatrices;
	}CASCADE_DATA;

	typedef struct tagCascadeConfig
	{
		vector<float>	Splits;
		float			fLamda;
		float			fBias;
		XMFLOAT4		vLightDir;
	}CASCADE_CONFIG;

	typedef struct tagSSAOConfig
	{
		unsigned int    iNumKernels;
		float			fRadius;
		float			fIntensity;
		float			fConstrast;
	}SSAO_CONFIG;

	typedef struct tagGaussianBlurConfig
	{
		int				iRadius;
		float			fSigma;
		float			fNormalization;
	}GAUSSIAN_BLUR_CONFIG;

	typedef struct tagFogNoise_DESC
	{
		bool			isEnable;
		XMFLOAT2		vSpeed;
		XMFLOAT2		vScale;
		float			fStrength;
		float			fContrast;
	}FOG_NOISE_DESC;

	typedef struct tagFogConfig
	{
		enum TYPE { LINEAR, EXP, EXPSQUARE };

		TYPE			eType;
		float			fNear, fFar;	// 선형용
		float			fDensity;		// 지수용
		XMFLOAT4		vColor;
		FOG_NOISE_DESC	Noise;
	}FOG_CONFIG;

	typedef struct tagPointInstanceParams
	{
		XMFLOAT4 vInitTranslation; 
		float fSize;
	}POINT_INSTANCE_PARAMS;

	typedef struct tagPointInstanceSpeedParams
	{
		XMFLOAT4			fSpeed;
		float				fGravity;
		float				bDead;
		XMFLOAT2			padding;
	}POINT_INSTANCE_SPEED_PARAMS;

	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOS;

	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "PSIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOINT;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOSTEX;

	typedef struct tagVertexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXCUBE;

	typedef struct tagVertexNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXNORTEX;

	typedef struct tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 5 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXMESH;

	typedef struct tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		/* 정점에게 적용되야할 뼈들의 인덱스*/
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXANIMMESH;

	typedef struct tagVertexInstanceMesh
	{
		XMFLOAT4			vRight{};
		XMFLOAT4			vUp{};
		XMFLOAT4			vLook{};
		XMFLOAT4			vTranslation{};

		unsigned int		iID{};
	}VTXINSTANCE_MESH;

	/* Point Instancing::Instance Buffer - [Effect] */
	typedef struct tagVertexInstanceParticle
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT3			vPrevPosition;
		float				bDead;
		XMFLOAT2			vLifeTime;
	}IB_POINTINSTANCE_EFFECT;

	/* Mesh Instancing::Instance Buffer - [Effect] */
	typedef struct tagMeshInstanceParticle
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT2			vLifeTime;
		float				bDead;
	}IB_MESHINSTANCE_EFFECT;

	/* Mesh Instancing::Vertex Buffer - [Effect] */
	typedef struct tagVertexMeshInstanceParticle
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;
	}VB_MESHINSTANCE_EFFECT;

	typedef struct tagVertexPointParticle
	{
		static const unsigned int	iNumElements = { 8 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, 
			{ "TEXCOORD", 1, DXGI_FORMAT_R32_FLOAT, 1, 76, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, 
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, 
		};
	}VTXPOINTPARTICLE;

	typedef struct tagVertexParticle
	{
		static const unsigned int	iNumElements = { 8 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}VTXPARTICLE;

	typedef struct tagMeshInstancing							// 메쉬 인스턴싱 쉐이더용 구조체
	{
		static const unsigned int iNumElements = { 10 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "ID", 0, DXGI_FORMAT_R32_UINT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

	}MESH_INSTANCING;

	typedef struct tagAnimMeshInstancing							// 메쉬 인스턴싱 쉐이더용 구조체
	{
		static const unsigned int iNumElements = { 12 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iNumElements] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "ID", 0, DXGI_FORMAT_R32_UINT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

	}ANIMMESH_INSTANCING;
	
	typedef struct tagVertexInstanceUI
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;

		XMFLOAT4		vUV;
		XMFLOAT4		vColor;
		float			fAlpha;
		unsigned int	iTexPass;
		unsigned int	iShaderPass;
	}VTXINSTANCE_UI;

	typedef struct tagUI_INSTANCING
	{
		static const unsigned int	iNumElements = { 9 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_UINT, 1, 100, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}UI_INSTANCING;


	/* 애니메이션 전환 조건 데이터*/
	typedef struct tagAnimationTransitionConditionDesc
	{
		ANIM_TRANSITIONTYPE type = ANIM_TRANSITIONTYPE::AUTO; //디폴트 값 :auto

		float fMinPlayTime = 0.f;           // 최소 재생 시간 (이 시간 이전에는 전환 안 됨)
		float fMaxPlayTime = 999.f;         // 최대 재생 시간 (타임아웃)

		/* Flag */
		wstring strflagName = L"";	//체크할 플래그 키 값 ex) "isRunning" : Walk -> Run
		//bool	isFlag =  true;		//원하는 전환 조건 true: 전환하겠다.

		/* Input */
		unsigned int iKeyCode = 0;		//DIK_키코드
		unsigned int iInputType = 0;	// 0 : down , 1: pressing , 2: up

		int	iNextAnimIndex = -1;	//전환 시 다음 애니메이션 인덱스 ( -1이면 사용 x)
		string			strNextAnimSet=""; //전환 시 다음 애니메이션 세트 키값(비어 있으면 사용 x )

	}ANIM_TRANSITION;

	/* 애니메이션 이벤트 데이터*/
	typedef struct tagAnimationEventDesc
	{
		enum TRIGGER_INDEX {
			ONCE = 0,
			ENTER = 1,
			EXIT = 2,
			CONTINUE = 3
		};
		string					strEventKey = "";					// 이벤트 키
		XMFLOAT2				vFrameRange = { 0.f, 0.f };			// x: start, y: end
		bool					isTriggered[4] = { false, false, false, false };       // 이번 재생에서 발동했는지

		bool                    isTriggerOnce = { false };          // 루프 시 한 번만 발동
		bool					isTriggerOnEnter = { true };        // 범위 진입 시 발동
		bool					isTriggerOnExit = { false };        // 범위 탈출 시 발동
		bool					isTriggerContinuous = { false };    // 범위 내에서 계속 발동
	}ANIM_EVENT;

	typedef struct tagFontVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}FONTVERTEX;

	typedef struct tagGlyphInfo
	{
		int iWidth, iHeight;
		int iBearingX, iBearingY;
		int iAdvance;
		float u0, v0, u1, v1;
		uint64_t lastUsedFrame = 0;
	}GLYPH_INFO;

	typedef struct tagCollsion
	{
		class CGameObject* pGameObject = nullptr;
		void* pInfo = nullptr;	
	}COLLISION_DESC;


	typedef struct tagDirectionInfo
	{
		enum DIR : uint32_t {
			NONE = 0, F = 1 << 0, B = 1 << 1, L = 1 << 2, R = 1 << 3,
			U = 1 << 4, D = 1 << 5,
			C = 1 << 6, CC = 1 << 7,
			ALL = 1 << 8,
			BBL = 1 << 9, BLL = 1 << 10,
			END = 1 << 11,
		};
		

		void		Add_Flag(unsigned int iFlag) { iDirFlag |= iFlag; }
		void		Delete_Flag(unsigned int iFlag) { iDirFlag &= ~iFlag; }
		void		Clear_Flag() { iDirFlag = 0; }
		bool		Check_Flag(unsigned int iFlag) { return (iDirFlag & iFlag) != 0; }

		unsigned int iDirFlag = {};

	}DIRECTION_INFO;

}


#endif // Engine_Struct_h__
