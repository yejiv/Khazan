#include "BlackBoard.h"
#include "GameObject.h"

CBlackBoard::CBlackBoard()
{

}

CBlackBoard* CBlackBoard::Create()
{
	return new CBlackBoard();
}

void CBlackBoard::Free()
{
	m_Data.clear();
	m_KeyTypes.clear();
}





