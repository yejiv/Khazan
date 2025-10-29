#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;

class COctree final : public CBase
{
	struct tInstance
	{
		class CGameObject* pGameObject;
		vector<pair<_float3, _float4x4>> InstanceInfo;		// vPoint, vWorldMatrix
		float fRadius;
	};

protected:
	enum CHILD
	{
		LBB, RBB, LTB, RTB, LBF, RBF, LTF, RTF, CHILDEND
		//x  -	  +    -    +    -    +    -    +
		//y  -    -    +    +    -    -    +    +
		//z  -    -    -    -    +    +    +    +
	};

public:
	COctree();
	virtual ~COctree();

public:
	HRESULT Initialize(const _float3& vCenter, const _float& fHalfWidth, const _int& iDepthLimit, COctree* pParent = nullptr);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Culling();

public:
	bool AddStaticObject(class CGameObject* pGameObject, const _float3& vPoint, const _float& fRadius = 0.0f);
	//bool AddStaticInstance(const string& _strModelTag, class CGameObject* _pGameObject, const _float3& _vPoint, const _float4x4& _WorldMatrix, const float& _fRadius = 0.0f);
	void Clear();
	void Destroy();

private:
	ContainmentType isDraw();
	void AllVisible();
	void Invisible();
	vector<tInstance>* FindInstance(const string& strModelTag);

private:
	BoundingBox m_BoundingBox{};
	COctree* m_pChilds[CHILDEND]{};
	COctree* m_pParent;
	int m_iDepth{ 0 };
	bool m_isVisible{ false };
	bool m_isObtainStatic{ false };

	list<class CGameObject*> m_GameObjects;

	//map<const string, tInstance> m_Instances;
	map<const string, vector<tInstance>> m_Instances;

	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static COctree* Create(const _float3& vCenter, const _float& fHalfWidth, const _int& iDepthLimit, COctree* pParent = nullptr);
	virtual void Free() override;
};

NS_END