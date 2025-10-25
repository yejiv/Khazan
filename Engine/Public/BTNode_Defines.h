#pragma once
#include "Engine_Defines.h"
#include "BlackBoard.h"
#include "Sequence_Node.h"
#include "Selector_Node.h"
#include "CoolDown_Nodel.h"
#include "Repeater_Node.h"
#include "Inverter_Node.h"
#include "Condition_Node.h"
#include "Action_Node.h"
#include "Wait_Node.h"


typedef function<_bool(class CBlackBoard*)> CONDITION;
typedef function<BTNODESTATE(CBlackBoard* BB)> ACTION;
typedef	function<void(CBlackBoard*, BTNODESTATE)> TERMINATE;