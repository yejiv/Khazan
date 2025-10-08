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

		unsigned int	iWinSizeX_Imgui, iWinSizeY_Imgui;
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

	typedef struct tagShadowLightDesc
	{
		XMFLOAT4		vEye, vAt;
		float			fFovy, fNear, fFar;
	}SHADOW_LIGHT_DESC;

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
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;		
	}VTXINSTANCE_MESH;

	typedef struct tagVertexInstanceParticle
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT2			vLifeTime;
	}VTXINSTANCE_PARTICLE;

	typedef struct tagVertexParticle		
	{
		static const unsigned int	iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}VTXPARTICLE;

	typedef struct tagVertexPointParticle
	{
		static const unsigned int	iNumElements = { 6 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },	

			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
	}VTXPOINTPARTICLE;



}


#endif // Engine_Struct_h__
