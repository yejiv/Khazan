#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
    enum MODEL_STATE
    {
        /* base */
        ANIM_LOOP = 1 << 0,
        USED_ANIM_LOOP = 1 << 1,
        CHANGE_ANIMATION = 1 << 2,	//m_isChangedAnimation
        FIRST_FRAME_ANIMATION = 1 << 3,
        WAITFORCOMPLETE = 1 << 4,	//애니메이션 완료되어야 다음 애니메이션 전환 가능 

        /* animation set */
        ANIMSET_PLAYING = 1 << 5,
        ANIMSET_NEXT = 1 << 6, //m_isSetAnimNextPlay

        /* root motion*/
        ROOTMOTION = 1 << 7,
        ROOTMOTION_POSITION = 1 << 8,
        IGNORE_ROOT_ROT = 1 << 9,
        IGNORE_ROOT_POS = 1 << 10,
        IGNORE_ROOT_POS_FIRSTFRAME = 1 << 11,
        ABSOLUTE_ROOT_POS = 1 << 12,
        ROOTMOTION_ALL = ROOTMOTION | ROOTMOTION_POSITION | IGNORE_ROOT_ROT | IGNORE_ROOT_POS | IGNORE_ROOT_POS_FIRSTFRAME | ABSOLUTE_ROOT_POS,

        /* event */
        EVENT = 1 << 13,



    };

    typedef struct tagRootMotionInfo
    {
        _vector			vScale = {};	//루트모션 포지션 값 어디 축에만 적용할 것인지.
        _vector			vFirstFrameOffset = {}; //첫 프레임 오프셋 저장. 콤보애니같은경우 위치값이 크므로.
        _vector			vDeltaQuat = {};
        _vector			vPreTransformQuat = {}; //-0
        _matrix			matPreRootMotion = {}; // 이전 루트 모션 행렬	
        _matrix			matDeltaRootMotion = {};// 루트모션 변화량

    }ROOTMOTION_INFO;

    typedef struct tagAnimationSetInfo
    {
        _uint			iCurrentIndex = { 0 };// 애님세트가 진행중인데 몇번째인지? 
        _uint			iTotalCount = { 0 }; //애님 세트에서 총 몇개의 애님이 있는지
        _uint			iSelectedAnimIndex = { 0 }; // 키값에 해당하는 어떤 애니메이션인지
    }ANIMATIONSET_INFO;

private:
    CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, _bool isSRVCache = false);
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual HRESULT Render(_uint iMeshIndex);
    virtual HRESULT Deferred_Render(_uint iMeshIndex, ID3D11DeviceContext* pDeferredContext);

public:
    HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iIndex);
    HRESULT Bind_Materials(class CDeferredShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iIndex);
    HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
    HRESULT Bind_PrevBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

public:
    // 권성은 테스트
    _float				MakeRatio();
    _float3				Get_RootMotionDelta() const { return m_vDelta; }
    ROOTMOTION_INFO		Get_RootMotion_Info() const { return m_RootMotionInfo; }
    void                Set_BlendTime(_float fBlendTime);

    /* 정보 */
    _uint				Get_NumMeshes() const { return m_iNumMeshes; }
    _uint				Get_BoneIndex(const _char* pBoneName);
    _float4x4* Get_BoneMatrix(const _char* pBoneName);
    _float4x4* Get_BoneMatrix(_int iBoneIndex);
    _float4x4* Get_LocalBoneMatrix(const _char* pBoneName);
    _float4x4* Get_LocalBoneMatrix(_int iBoneIndex);
    _float4x4* Get_ContainNameBoneMatrix(const _char* pBoneName);
    _int				Get_CurAnimIndex() { return m_iCurrentAnimIndex; }
    _int				Get_AnimIndexByName(const string& strName);
    string				Get_CurAnimName() { return m_AnimationsSetup[m_iCurrentAnimIndex].strName; }
    _float* Get_CurTrackPosition() { return &m_fCurrentTrackPosition; }
    const vector<_int>& Get_ChildIndices(_int boneIndex) const;
    _float				Get_CurDuration();
    _bool				IsFinished() { return m_isFinished; }

    void				Set_RootBone(_uint iIndex) { m_iRootBoneIndex = iIndex; }
    void				Set_OwnerTransform(class CTransform** pTransform);
    void                Set_Transform(const _float4x4* pMat) { m_pTransformMatrix = pMat; }
    void				Set_OwnerTransform(_float4x4* pMatrix) { m_pOwnerTransformMatrix = pMatrix;}
    _float4x4*          Get_OwnerWorldMatrix() const {return m_pOwnerTransformMatrix;}
    _bool               IsAnimationStart(_uint iAnimIndex) { return iAnimIndex == m_iCurrentAnimIndex && m_isAnimStart; } //해당 애니메이션이 시작 됐는지 

    _vector Get_BoneWorldRotationQuat(_int iBone) const;

    void Set_BoneLocalRotation(_int iBone, _vector vLocal);
    void Set_BoneWorldRotation(_int iBone, _vector vWorld);
    void Set_PreTransformMatrix(_float4x4 PreTransformMatrix) { m_PreTransformMatrix = PreTransformMatrix; }
    vector<_float3>		Get_VerticesPos(_uint iIndex);		//졸트
    vector<_uint>		Get_Indices(_uint iIndex);			//졸트

    /* 애니메이션 기능  */
    _bool			Play_Animation(_float fTimeDelta);
    void			Set_Animation(_uint iIndex);
    void			Set_AnimationSet(const string& strKey);
    void			Set_AnimationLoop(_bool isLoop);
    _bool			Check_MinAnimationTime();
    _bool           Check_CanDodgeTime();
    void            AnimationSetIndexIncrease(); //애니메이션세트 강제로 다음으로 넘기기
    void            Set_AnimationBlend(_bool isBlend) { m_isBlendEnable = isBlend; }      // 애니메이션 보간할건지 여부
    void            AnimationLoop(_bool isLoop);
    void            Set_AnimBlendTime(_uint iAnimIndex, _float fBlendTime);

    /* rootBone Combined  */
    void			Update_BoneCombinedMatrices();

    /* 이벤트 */
    void			Register_Event(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType, function<void()> OnEvent);
    void			UnRegister_Event(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType);
    void			Clear_AllEvent();

    /* 스켈레톤 공유 */
public:
    void			Set_MasterSkeleton(CModel* pMaster);
    void			Attach_Part(CModel* pPart);
    void			Detach_Part(CModel* pPart);
    void			Render_AllAttachedParts();
    const			vector<_float4x4>& Get_PartLocalBoneMatrices() const { return m_PartLocalBoneMatrices; }
    void            Build_PartToMasterMap();
    void			Update_PartLocalBones();
    
public: 
    /* 모든 뼈 정보 */
    const vector<_float4x4>& Get_CachedBoneMatrices() const { return m_CachedBoneMatrices; }

    /* After Image */
    void Capture_CurrentFrameMatrices(vector<_float4x4>& OutBoneMatrices, _float4x4* pOutWorldMatrix);
    _bool Restore_Frame(const vector<_float4x4>& SnapshotBoneMatrices);  // 몇 프레임 전으로 복원

    /* 모든 뼈 정보 저장 */
    void            Cache_CurrentBoneMatrices();
    /* 모든 뼈 정보 복원 */
    void            Restore_CurrentBoneMatrices();


    /* 임시 */
    void            WarmupAnimations();
#ifdef _DEBUG
public:
    void			Debug_RanderState();
#endif // _DEBUG

private:
    /* Info*/
    class CTransform* m_pOwnerTransform = { nullptr };
    _float4x4* m_pOwnerTransformMatrix = { nullptr };
    _uint								m_iState = { 0 };
    _wstring							m_strModelName{};
    _wstring							m_strModelFilePath{};
    MODELTYPE							m_eModelType = {};
    _float4x4							m_PreTransformMatrix = {};
    const _float4x4*					m_pTransformMatrix = {};    //나의 트랜스폼
    _uint								m_iRootBoneIndex = { 0 };

    /* 매쉬 */
    vector<class CMesh*>				m_Meshes;
    _uint								m_iNumMeshes = {};

    /* 머터리얼 */
    vector<class CMeshMaterial*>		m_Materials;
    _uint								m_iNumMaterials = {};

	/* 뼈 */
	vector<class CBone*>				m_Bones;
    vector<_float4x4>                   m_CachedBoneMatrices;  // 캐싱용(애니메이션이 끝난 모든 뼈정보를 저장)
    vector<_float4x4>                   m_CachedPrevBoneMatrices;

    /* 애니메이션 */
    _bool                               m_isAnimStart = { false };          /* 해당 애니메이션이 시작되었는지  Set_Animation만 하고 아직 실행 안했을 수도 있음 */
    _bool								m_isFinished = { false };			/* 루프가 아닌 애니메이션이 끝났는지 여부  */
    _uint								m_iNumAnimations = { 0 };			/* 애니메이션 수 */
    _int								m_iCurrentAnimIndex = { 0 };		/* 현재 애니메이션 인덱스 */
    _int								m_iReserveAnimIndex = {};			/* 애니메이션 끝나고 전환해야 할 때 다음 애니메이션 인덱스가 들어오는것을 예약*/
    _float								m_fCurrentTrackPosition = { 0.f };  /* 현재 애니메이션의 현재 재생 위치 */
    _float								m_fPrevTrackPosition = { 0.f };		/* 현재 애니메이션의 이전 트랙재생 위치 */
    _int								m_iPrevAnimIndex = { 0 };			/* 이전 애니메이션 인덱스 */
    vector< class CAnimation* >			m_Animations;						/* 애니메이션 클래스 저장  */
    vector<ANIMATION_SETUP_DATA>		m_AnimationsSetup;					/* 애니메이션들 정보 */
    //vector<_bool>						m_AnimationFinished;				/* 애니메이션들이 끝났는지 정보 */
    _bool                               m_isBlendEnable = { true };         /* 애니메이션 보간할건지 여부 ( 기본값 true ) */

    /* 애니메이션 세트 */
    vector< ANIMATION_SET_DATA >		m_AnimationSets;					/* 애니메이션 세트 정보*/
    ANIMATIONSET_INFO					m_AnimationSetInfo;					/* 애니메이션 세트 진행 정보 */

    /* 루트 모션 */
    ROOTMOTION_INFO						m_RootMotionInfo;					/* 루트 모션 정보 */


    /* 애니메이션 완료 후 다음 애니메이션으로 */
    function<void()>					m_OnWaitForComplete = { nullptr };

    /* 이벤트  */
    unordered_map<string, function<void()>> m_EventCallbacks;	//콜백 관리
    vector<ANIM_EVENT>						m_CurrentEvents;	//현재 이벤트 상태 추적
    vector<_bool>							m_PrevFrameInRange;	//이전 프레임에 이벤트 발동 했는지

    /* 스켈레톤 공유 */
    CModel* m_pMasterSkeleton = { nullptr };
    vector<CModel*>						m_AttachedParts;
    vector<_float4x4>					m_PartLocalBoneMatrices;  //파츠 로컬 본 행렬
    _bool								m_isMaterSkeleton = { false };
    _bool								m_isSharedSkeleton = { false };

	/* const val */
	const _float					    m_fBaseRootMotionBlendTime = { 0.15f };   /* 만약 블랜딩 시간이 안써져있으면 사용할 기본 블랜딩 시간 */
	_float3							    m_vDelta = {};

    /* 최적화용 */
    vector<_int>      m_PartToMasterIndex;     // [partBone] = masterBoneIndex or -1

    _bool isChanged = {};

private:
    /* 루트 모션 */
    void			Check_RootMotion();
    void			Update_RootMotion(_float fTimeDelta);
    void			Apply_RootMotion_To_Transform();

    /* 완료 여부 */
    void			Check_WaitForComplete();
    void			OnWaitForComplete(function<void()> onEvent) { m_OnWaitForComplete = onEvent; }

    /* 이벤트 */
    void			Setup_Events();
    void			Check_Event(_float fTimeDelta);
    void			Trigger_Event(string strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType);
    void			Reset_EventTrigger();
    string			MakeCallbackKey(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType);

private:
    HRESULT Ready_Meshes(MODEL_DATA& data);
    HRESULT Ready_Materials(MODEL_DATA& data, _bool isSRVCache = false);
    HRESULT Ready_Bones(MODEL_DATA& data);
    HRESULT Ready_Animations(MODEL_DATA& data);

private:
    inline void		Add_State(MODEL_STATE s) { m_iState |= s; }
    inline void		Add_State(_uint i) { m_iState |= i; }
    inline void		Remove_State(MODEL_STATE s) { m_iState &= ~s; }
    inline void		Remove_State(_uint i) { m_iState &= ~i; }
     inline _bool	Has_State(MODEL_STATE s) { return (m_iState & s) != 0; }
    inline _bool	Has_State(_uint i) { return (m_iState & i) != 0; }
    inline _bool	Has_AllStates(_uint i) { return (m_iState & i) == i; }
    inline void		Clear_State() { m_iState = 0; }

public:
    inline void DFS_BuildChainsFromRoot(_int cur, _int maxDepth, vector<_int>& path, vector<vector<_int>>& outChains, _int minLen /*=2*/);
    inline vector<vector<_int>> BuildChainsFromRoot(_int rootBone, _int maxDepth /*=5*/, _int minLen /*=2*/);



public:
    static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _bool isSRVCache = false);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

};

NS_END
