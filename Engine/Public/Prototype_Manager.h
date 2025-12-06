#pragma once

/* 원형으로 추가될 클래스 타입을 확인할 수 있도록 모든 종류의 원형 클래스 헤더를 인클루드 해놓는다. */
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_LineTrail.h"
#include "VIBuffer_QuadTrail.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Point.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "Navigation.h"
#include "Transform.h"
//#include "Collider.h"
#include "Texture.h"
#include "Shader.h"
#include "DeferredShader.h"
#include "Model.h"
#include "Model_Instance.h"
#include "RigidBody.h"
#include "CharacterVirtual.h"
#include "Body.h"
#include "Texture_Atlas.h"
#include "VIBuffer_Instance_UI.h"
#include "Decal.h"
#include "Model.h"
#include "MotionTrail.h"
#include "SoftBody.h"
#include "ClothBody.h"
/* 원형객체(게임오브젝트, 컴포넌트)를 레벨별로 구분하여 보관한다. */
/* 복제하여 만들어진 사본객체를 리턴. */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

public:
	// 맵 툴에서 사용하는 용도입니다 . . . ( 프로토 타입 등록되어있나 확인 여부 ) ( BOOL 값 반환 )
	_bool Already_Registered_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);

	// 맵 툴에서 사용하려고 만든 함수입니다 . . . ( 프로토 타입 모델 프리뷰 )
	class CBase* Find_Prototype_ForPreview(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);

private:
	_uint											m_iNumLevels = {  };
	map<const _wstring, class CBase*>*				m_pPrototypes = { nullptr };
	typedef map<const _wstring, class CBase*>		PROTOTYPES;
	

private:
	class CBase* Find_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);


public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

};

NS_END