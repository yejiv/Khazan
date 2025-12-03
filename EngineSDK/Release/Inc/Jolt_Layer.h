#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CJolt_BPLayerIF final : public BroadPhaseLayerInterface
{
public:
	explicit CJolt_BPLayerIF(_uint iNumObjectLayer) {
		m_iNumObjectLayer = iNumObjectLayer;
        m_ObjectToBroadPhase = new BroadPhaseLayer[m_iNumObjectLayer];
	}
	virtual ~CJolt_BPLayerIF() {
		Safe_Delete_Array(m_ObjectToBroadPhase);
	}

public:
	virtual _uint	GetNumBroadPhaseLayers() const override { 
		return ENUM_CLASS(JOLT_BP_LAYER::END); 
	}
	virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		assert(inLayer < m_iNumObjectLayer);
		return m_ObjectToBroadPhase[inLayer];
	}
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING):	return "NON_MOVING";
		case ENUM_CLASS(JOLT_BP_LAYER::MOVING):		return "MOVING";
		case ENUM_CLASS(JOLT_BP_LAYER::TRIGGER):	return "TRIGGER";
		default:
			return "UNKNOWN";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	void			SetUp_ObjectToBP(_uint iObjectLayer, _uint iBPLayer)
	{
		assert(iObjectLayer < m_iNumObjectLayer && iBPLayer < ENUM_CLASS(JOLT_BP_LAYER::END));
		m_ObjectToBroadPhase[iObjectLayer] = BroadPhaseLayer(iBPLayer);
	}

private:
	_uint						m_iNumObjectLayer = {};
	BroadPhaseLayer*			m_ObjectToBroadPhase = { nullptr };
};

class CJolt_ObjectLayerPairFilter : public ObjectLayerPairFilter
{
public:
	explicit CJolt_ObjectLayerPairFilter(_uint iNumObjectLayer) {
		m_iNumObjectLayer = iNumObjectLayer;
		m_ObjectLayerFilter = new _bool*[m_iNumObjectLayer];
		for (_uint i = 0; i < m_iNumObjectLayer; ++i)
            m_ObjectLayerFilter[i] = new _bool[m_iNumObjectLayer]{};
	}
	virtual ~CJolt_ObjectLayerPairFilter() {
		for (_uint i = 0; i < m_iNumObjectLayer; ++i)
			Safe_Delete_Array(m_ObjectLayerFilter[i]);
		Safe_Delete_Array(m_ObjectLayerFilter);
	}

public:
	virtual _bool					ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		 assert(inObject1 < m_iNumObjectLayer && inObject2 < m_iNumObjectLayer);
		return m_ObjectLayerFilter[inObject1][inObject2];
	}

	void			SetUp_ObjectFilter(_uint iSrc, _uint iDst)
	{
		assert(iSrc < m_iNumObjectLayer && iDst < m_iNumObjectLayer);
		m_ObjectLayerFilter[iSrc][iDst] = true;
		m_ObjectLayerFilter[iDst][iSrc] = true;
	}

private:
	_uint	m_iNumObjectLayer = {};
	_bool** m_ObjectLayerFilter = { nullptr };
};

class CJolt_ObjectVsBPLayerFilter : public ObjectVsBroadPhaseLayerFilter
{
public:
	explicit CJolt_ObjectVsBPLayerFilter(_uint iNumObjectLayer) {
		m_iNumObjectLayer = iNumObjectLayer;
		m_ObjectVsBPLayerFilter = new _bool*[m_iNumObjectLayer];
        for (_uint i = 0; i < m_iNumObjectLayer; ++i)
            m_ObjectVsBPLayerFilter[i] = new _bool[ENUM_CLASS(JOLT_BP_LAYER::END)]{};
	}
	virtual ~CJolt_ObjectVsBPLayerFilter() {
		for (_uint i = 0; i < m_iNumObjectLayer; ++i)
			Safe_Delete_Array(m_ObjectVsBPLayerFilter[i]);
		Safe_Delete_Array(m_ObjectVsBPLayerFilter);
	}

public:
	virtual _bool					ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		assert(inLayer1 < m_iNumObjectLayer && inLayer2.GetValue() < ENUM_CLASS(JOLT_BP_LAYER::END));
		return m_ObjectVsBPLayerFilter[inLayer1][inLayer2.GetValue()];
	}

	void			SetUp_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer)
	{
		assert(iObjectLayer < m_iNumObjectLayer && iBPLayer < ENUM_CLASS(JOLT_BP_LAYER::END));
		m_ObjectVsBPLayerFilter[iObjectLayer][iBPLayer] = true;
	}

private:
	_uint						m_iNumObjectLayer = {};
	_bool** m_ObjectVsBPLayerFilter = { nullptr };
};
class CJolt_ObjectLayerFilter final : public JPH::ObjectLayerFilter
{
public:
	explicit CJolt_ObjectLayerFilter(_uint ilayers)
		: m_iNumObjectLayer(ilayers), m_iAllow(ilayers, false) {
	}   // 전부 false로 초기화

// 이 레이어를 캐스트 대상으로 허용할지?
	bool ShouldCollide(JPH::ObjectLayer layer) const override
	{
		return layer < m_iNumObjectLayer && m_iAllow[layer];
	}

	// 허용/비허용 설정
	void SetUpAllow(JPH::ObjectLayer layer, bool isOn = true)
	{
		if (layer < m_iNumObjectLayer) m_iAllow[layer] = isOn;
	}

private:
	_uint                 m_iNumObjectLayer{};
	vector<_bool>    m_iAllow; 
};
NS_END