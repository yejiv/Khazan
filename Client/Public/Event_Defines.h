#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { TEST, UI_QUICK_SLOT, END };


	// Structs
	struct EventTest {
		int data;
	};

	struct EVENT_HUD_QUICKSLOT {
		_bool isEquip;
		_int  iIndex;
		_int  iItemIndex;
		_int* iItemCount;
	};
}