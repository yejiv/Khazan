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
	void							Edit_Options();
	void							Create_Box_Spawn();
	void							Create_Circle_Spawn();
	void							GetParticleColor();

	void							Create_PointInstance_Element();
	void							Create_MeshInstance_Element();
	void							Create_Sprite_Element();

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

	//[Sprite Data]
	_uint							m_iCol, m_iRow;
	_float							m_fSpriteSpeed;
	_float							m_fScalingValue;

	CEffect_Prefab::EFFECT_EVENT	m_WorkingTrackData;

	_int							m_iChildrenIdx;
	_int							m_iPrevChildrenIdx;

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_GameObject();

public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END