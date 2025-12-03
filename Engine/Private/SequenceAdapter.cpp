//#include "ImSequencer.h"
//#include "Engine_Defines.h"
//
//NS_BEGIN(Engine)
//
//class MySequenceAdapter : public ImSequencer::SequenceInterface
//{
//public:
//    struct Clip
//    {
//        int         type = 0;
//        int         start = 0;
//        int         end = 0;        // end >= start
//        uint32_t    color = 0xFF4AA3FF; // ARGB(또는 AARRGGBB)
//        std::string label = "New";
//    };
//public:
//    // --- 타임라인 전체 프레임 범위 ---
//    int GetFrameMin() const override { return mFrameMin; }
//    int GetFrameMax() const override { return mFrameMax; }
//
//    // --- 아이템 개수 ---
//    int GetItemCount() const override { return (int)mClips.size(); }
//
//    // --- 아이템 속성 접근 (중요: start/end는 int** 로 넘겨야 함) ---
//    // Get(i, &startPtr, &endPtr, &type, &color);
//    void Get(int index, int** start, int** end, int* type, unsigned int* color) override
//    {
//        Clip& c = mClips[index];
//        if (start) *start = &c.start;
//        if (end)   *end = &c.end;
//        if (type)  *type = c.type;
//        if (color) *color = c.color;
//    }
//
//    // --- 라벨 표시 ---
//    const char* GetItemLabel(int index) const override
//    {
//        return mClips[index].label.c_str();
//    }
//
//    // --- 타입 메뉴/추가 ---
//    int GetItemTypeCount() const override { return (int)mTypeNames.size(); }
//    const char* GetItemTypeName(int typeIndex) const override
//    {
//        return mTypeNames[typeIndex].c_str();
//    }
//    void Add(int type) override
//    {
//        Clip c;
//        c.type = type;
//        c.start = mFrameMin;
//        c.end = mFrameMin + 30;
//        c.color = 0xFF59C36A;
//        c.label = std::string(mTypeNames[type]) + " Clip";
//        mClips.push_back(std::move(c));
//    }
//
//    // --- 삭제/복제/더블클릭/복붙 ---
//    void Del(int index) override
//    {
//        if ((size_t)index < mClips.size())
//            mClips.erase(mClips.begin() + index);
//    }
//    void Duplicate(int index) override
//    {
//        if ((size_t)index < mClips.size())
//        {
//            Clip c = mClips[index];
//            int len = c.end - c.start;
//            c.start += len + 2;
//            c.end += len + 2;
//            c.label += " (copy)";
//            mClips.push_back(std::move(c));
//        }
//    }
//    void DoubleClick(int /*index*/) override
//    {
//        // 예: 속성 패널 열기 트리거
//    }
//    void Copy() override { /* 필요시 구현(클립보드/로컬 버퍼) */ }
//    void Paste() override { /* 필요시 구현 */ }
//
//    // --- 편집 세션 훅(Undo 그룹핑 등) ---
//    void BeginEdit(int /*index*/) override { /* BeginUndo("SeqEdit"); */ }
//    void EndEdit()                override { /* EndUndo();            */ }
//
//    // --- 접힘 상태일 때 헤더 텍스트 포맷 ---
//    const char* GetCollapseFmt() const override
//    {
//        // format: frameCount, itemCount
//        return "%d frames / %d items";
//    }
//
//    // --- 커스텀 라인 높이 & 커스텀 드로우 (필요 없으면 0과 빈 구현) ---
//    size_t GetCustomHeight(int /*index*/) const override { return 0; }
//    void CustomDraw(
//        int /*index*/, ImDrawList* /*dl*/,
//        const ImRect& /*customRect*/, const ImRect& /*legendRect*/,
//        const ImRect& /*clipRect*/, const ImRect& /*legendClipRect*/) override
//    {
//        // 필요시: 추가 시각화
//    }
//    void CustomDrawCompact(
//        int /*index*/, ImDrawList* /*dl*/,
//        const ImRect& /*customRect*/, const ImRect& /*clipRect*/) override
//    {
//        // 압축 모드용 그리기
//    }
//
//public:
//    // Sequencer 가 직접 만지는 포커스 플래그
//    bool focused = false;
//
//    // 네가 원하는 초기값으로 세팅
//    int mFrameMin = 0;
//    int mFrameMax = 600;
//
//    std::vector<Clip> mClips = {
//        {0,   0, 120, 0xFF59C36A, "Intro"},
//        {1, 140, 240, 0xFFE5A14D, "Camera"},
//        {2, 260, 340, 0xFF4AA3FF, "FX"}
//    };
//    std::vector<std::string> mTypeNames = { "Generic", "Camera", "FX" };
//};
//
//NS_END