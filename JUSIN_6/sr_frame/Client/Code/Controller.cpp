#include "stdafx.h"
#include "Controller.h"

CController::CController()
{
}

CController::CController(const CController& rhs) : CComponent(rhs)
{
}

CController::~CController()
{
}

CComponent* CController::Clone()
{
	return nullptr;
}

void CController::Free()
{
	CComponent::Free();
}


