#include "stdafx.h"
#include "..\public\Skill_Manager.h"

#include "GameInstance.h"
#include "Obj_Manager.h"

#include "S_StunChick.h"

IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager()
{
}

void CSkill_Manager::StunChick(_float3 f3Pos, _bool bStun)
{
	//bStrun_One = bStun;

	//if (bStrun_One)
	//{
	//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);



	//	RELEASE_INSTANCE(CGameInstance);

	//	bStrun_One = false;
	//}
}

void CSkill_Manager::Free()
{
}