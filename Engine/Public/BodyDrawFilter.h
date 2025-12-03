#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class DrawOnlyLayers final : public JPH::BodyDrawFilter
{
public:
    DrawOnlyLayers()
    {
    }

    bool ShouldDraw(const JPH::Body& b) const override
    {
        return mLayers.find(b.GetObjectLayer()) != mLayers.end();
    }

    void SetUp_LayerToFilter(_uint iObjectLayer)
    {
        mLayers.insert((ObjectLayer)iObjectLayer);
    }

    _bool Remove_LayerToFilter(_uint iObjectLayer)
    {
        const auto key = static_cast<ObjectLayer>(iObjectLayer);
        auto it = mLayers.find(key);
        if (it != mLayers.end())
        {
            mLayers.erase(it);        // 또는 mLayers.erase(key); (동일 효과)
            return true;              // 지웠음
        }
        return false;                 // 없었음
    }

private:
    std::unordered_set<JPH::ObjectLayer> mLayers;
};

NS_END