#ifndef Engine_Sequence_h__
#define Engine_Sequence_h__


namespace Engine {

    //Struct
    // 재생 인스턴스 식별 (한 시퀀스를 여러 번 동시 재생 대비)
    struct SEQ_ID {
        unsigned int iSeq = 0;
        unsigned int iInstance = 0;
    };

    // ===== 요청(Request) 페이로드 =====

    struct SEQ_REQ_PLAY_DESC
    {
        SEQ_ID tId{};  // iSeq는 필수, iInstance는 0이면 실행 시 자동 부여
        const wchar_t* pAsset = nullptr; // 데이터 드리븐용
        float fStartTime = 0.f; // 시작 오프셋
        bool isSkippable = true; // 스킵 가능 여부
        unsigned int iPriority = 0; // 컷신 우선순위(카메라/입력 락 정책용)
    };

    struct SEQ_REQ_PAUSE_DESC {
        SEQ_ID tId{};
    };

    struct SEQ_REQ_RESUME_DESC {
        SEQ_ID tId{};
    };

    struct SEQ_REQ_STOP_DESC
    {
        SEQ_ID tId{};
        bool bImmediate = true; // true면 즉시 중단(페이드 없이)
    };

    struct SEQ_REQ_JUMP_DESC
    {
        SEQ_ID tId{};
        float fTime = 0.f;  // 절대 타임라인 시각
        const wchar_t* pMarker = nullptr; // 마커명 지정 시 우선
    };

    // ===== 신호(Signal) 페이로드 =====
    struct SEQ_SIG_COMMON {
        SEQ_ID tId{};
    };

    struct SEQ_SIG_MARKER
    {
        SEQ_ID tId{};
        unsigned iMarker = 0; // 내부 마커 ID(선택)
        const wchar_t* pName = nullptr; // 마커명
    };

    struct SEQ_SIG_COND
    {
        SEQ_ID tId{};
        unsigned int iCond = 0;
        bool isTrue = false;
    };

    // ===== 유틸 =======
    struct SEQ_KEY { 
        _uint a, b; 
    };
    struct KeyHash { 
        size_t operator()(const SEQ_KEY& k) const { 
            return (size_t(k.a) << 32) ^ size_t(k.b); 
        } 
    };
    struct KeyEq { 
        _bool operator()(const SEQ_KEY& x, const SEQ_KEY& y) const 
        { 
            return x.a == y.a && x.b == y.b; 
        } 
    };

    static inline SEQ_KEY _K(const SEQ_ID& id) 
    { 
        return { id.iSeq, id.iInstance }; 
    }

    //ENUM
    // 이벤트 타입 (Event_Manager의 iEventType 값으로 사용)
    enum SEQ_EVENT {
        SEQ_REQ_PLAY = 50000,
        SEQ_REQ_PAUSE = 50001,
        SEQ_REQ_RESUME = 50002,
        SEQ_REQ_STOP = 50003,
        SEQ_REQ_JUMP = 50004,
        
        SEQ_SIG_STARTED = 50100,
        SEQ_SIG_ENDED = 50101,
        SEQ_SIG_INTERRUPTED = 50102,
        SEQ_SIG_MARKER = 50103,
        SEQ_SIG_CONDITION = 50104,

    };
}
#endif