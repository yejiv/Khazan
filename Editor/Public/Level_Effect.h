#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "Effect_Prefab.h"

NS_BEGIN(Editor)

class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT					Initialize() override;
	virtual void					Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;


private:
	class CEffect_Prefab*			m_PrefabPrototype;

private:
	//[Main GUI]
	void							Init_GUI();

	//[Child GUI]
	void							Prefab_Info();
	void							Create_Element();
	void							Edit_Element_List();
	void							Edit_Time_Track();
	void							Save_Load();

	void							Create_Box_Spawn();
	void							Create_Circle_Spawn();
	void							GetParticleColor();
	void							GetMaksingScrollData();

	void							Create_PointInstance_Element();
	void							Create_MeshInstance_Element();
	void							Create_Sprite_Element();

	void							SelectFilePath();

private :

	//[Type]
	_int							m_SpawnType;
	_int							m_EffectType;
	_bool							m_bLoop;

	//[Basic Data]
	_uint							m_iInstanceNum;
	_float							m_fSize[2];
	_float							m_fSizeRatio;
	_float							m_fLifeTime[2];
	_float2							m_fScrollSpeed;

	//[BoundingBox Spawn Data]
	_float							m_fCenter[3];
	_float							m_fRange[3];

	//[Circle Spawn Data]
	_float							m_fOffset;

	//[Color Data]
	_float4							m_fColor;
	_int							m_iTextureIdx;
	_int							m_iMeshTypeIdx;

	//[Masking Data]
	_bool							m_bIsMaskScrolling;
	_int							m_iMaskTextureIdx;		//마스크 텍스쳐
	_float							m_bMaskScrollSpeed;		//마스크 스피드
	_bool							m_bIsScrollVertical;	//마스크 스크롤 방향 (상하 <-> 좌우)
	_bool							m_bScrollDir;			// 왼->오, 위-> 아래가 기본인데 이거 체크되어있으면 반대로!

	//[Sprite Data]
	_uint							m_iCol, m_iRow;
	_float							m_fSpriteSpeed;

	//[Turbulence]
	_bool							m_bIsTurbulence;
	_uint							m_iTurbulenceTextureIdx;
	_float							m_fTurbulenceSpeed;
	_float							m_fTurbulenceSampleSize;

	//[Fresnel]
	_bool							m_bIsFresnel;

	_int							m_iChildrenIdx;
	_int							m_iPrevChildrenIdx;

	//[Dissolve]
	_bool							m_bIsDissolve;
	_int							m_iDissolveTextureIdx;
	_float							m_fDissolveEdgeWidth;
	_float4							m_fDissolveEdgeColor;

	//[Rotation]
	_float							m_fRotation[3];

	//[Save / Load]
	char							FilePath[128];
	char							SaveFileName[128] {};
	char							LoadFileName[128] {};

	/* [Debug] */
private:
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {};
	_tchar					m_szFPS[MAX_PATH] = {};

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_GameObject();

private:
	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 현재 로드할 레벨 | 4. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 현재 로드할 레벨 | 4. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 파라미터 ( 1. 데이터 파일 이름 | 2. 현재 로드할 레벨 | 3. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);


public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END