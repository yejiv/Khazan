#pragma once
#include "Engine_Defines.h"

namespace Client
{
	// struct
	struct ClipRange {
		_float fBegin;
		_float fEnd;
	};

	struct Clip
	{
		ClipRange tRange;
	};

	struct CameraClip
	{
		_wstring strMoveTag;
		function<void> fn;
	};

	struct PlayerClip
	{
		_wstring strMoveTag;
		function<void> fn;
	};



	//function
	inline bool InRange(float fTime, const ClipRange& tClip)
	{
		return fTime >= tClip.fBegin && fTime < tClip.fEnd;
	}
}