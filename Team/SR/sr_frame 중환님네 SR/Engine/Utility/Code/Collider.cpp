#include "Collider.h"
#include "CollisionCom.h"
#include "Transform.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(CCollider)

const BLOCKING_TYPE CCollider::s_BlockingTypeMatrix[COLL_END][COLL_END]
{    // {player ,eneymy, player bullet, enemy bullet,  wall
	{BLOCKING_END, BLOCKING_END, BLOCKING_END, BLOCKING_END, BLOCKING_END},
	{PUSH_OTHER_ONLY, PUSH_EACH_OTHER, BLOCKING_END, BLOCKING_END, BLOCKING_END},
	{BLOCKING_END, CUSTOM_EVENT, BLOCKING_END, BLOCKING_END, CUSTOM_EVENT},
	{CUSTOM_EVENT, BLOCKING_END, BLOCKING_END, BLOCKING_END, CUSTOM_EVENT},
	{PUSH_OTHER_ONLY, PUSH_OTHER_ONLY, BLOCKING_END, BLOCKING_END, BLOCKING_END},
};

CCollider::CCollider()
{
	Ready_Collider();
}


CCollider::~CCollider()
{
	Free();
}

void CCollider::Ready_Collider()
{
	m_vecGrid.resize(COLL_GRID_Z);
	for (auto& e : m_vecGrid)
		e.resize(COLL_GRID_X);

	m_fGridCZ = (_float)VTXCNTZ / COLL_GRID_Z;
	m_fGridCX = (_float)VTXCNTX / COLL_GRID_X;
}

void CCollider::Add_CollisionCom(CCollisionCom* pCollision)
{
	NULL_CHECK(pCollision);

	const _vec3 vPos = pCollision->GetTransform()->m_vInfo[INFO_POS];
	const _float fRadius = pCollision->GetRadius();

	_int iX_start = (_int)((vPos.x - fRadius) / m_fGridCX);
	_int iZ_start = (_int)((vPos.z - fRadius) / m_fGridCZ);
	if (iX_start < 0) iX_start = 0;
	if (iZ_start < 0) iZ_start = 0;

	_int iX_end = (_int)((vPos.x + fRadius) / m_fGridCX);
	_int iZ_end = (_int)((vPos.z + fRadius) / m_fGridCZ);
	if (iX_end >= COLL_GRID_X) iX_end = COLL_GRID_X - 1;
	if (iZ_end >= COLL_GRID_X) iZ_end = COLL_GRID_Z - 1;

	for (int i = iZ_start; i <= iZ_end; ++i)
	{
		for (int j = iX_start; j <= iX_end; ++j)
		{
			m_vecGrid[i][j].dynamicList.push_back(pCollision);
			pCollision->AddRef();
		}
	}
}

void CCollider::Add_StaticCollision(const _vec3& vCenter, _float fRadius)
{
	_int iX_start = (_int)((vCenter.x - fRadius )/ m_fGridCX);
	_int iZ_start = (_int)((vCenter.z - fRadius) / m_fGridCZ);
	if (iX_start < 0) iX_start = 0;
	if (iZ_start < 0) iZ_start = 0;

	_int iX_end = (_int)((vCenter.x + fRadius) / m_fGridCX);
	_int iZ_end = (_int)((vCenter.z + fRadius) / m_fGridCZ);
	if (iX_end >= COLL_GRID_X) iX_end = COLL_GRID_X - 1;
	if (iZ_end >= COLL_GRID_X) iZ_end = COLL_GRID_Z - 1;

	for (int i = iZ_start; i <= iZ_end; ++i)
	{
		for (int j = iX_start; j <= iX_end; ++j)
		{
			m_vecGrid[i][j].staticList.push_back({vCenter, fRadius});
		}
	}
}

void CCollider::GetOverlappedObject(set<CGameObject*>& objList, const _vec3& vPos, _float fRadius)
{
	objList.clear();
	_int iX_start = (_int)((vPos.x - fRadius) / m_fGridCX);
	_int iZ_start = (_int)((vPos.z - fRadius) / m_fGridCZ);
	if (iX_start < 0) iX_start = 0;
	if (iZ_start < 0) iZ_start = 0;

	_int iX_end = (_int)((vPos.x + fRadius) / m_fGridCX);
	_int iZ_end = (_int)((vPos.z + fRadius) / m_fGridCZ);
	if (iX_end >= COLL_GRID_X) iX_end = COLL_GRID_X - 1;
	if (iZ_end >= COLL_GRID_X) iZ_end = COLL_GRID_Z - 1;

	for (int i = iZ_start; i <= iZ_end; ++i)
	{
		for (int j = iX_start; j <= iX_end; ++j)
		{
			for (auto& dynamic : m_vecGrid[i][j].dynamicList)
			{
				if (IsCollided(
					dynamic->GetTransform()->m_vInfo[INFO_POS], dynamic->GetRadius(),
					vPos, fRadius))
				{
					objList.insert(dynamic->GetOwner());
				}
			}
		}
	}
}

void CCollider::Check_Blocking()
{
	for(auto& vecRow : m_vecGrid)
	{
		for (auto& cell : vecRow)
		{
			for (size_t i = 0; i < cell.dynamicList.size(); ++i)
			{
				CCollisionCom*& coll_1 = cell.dynamicList[i];

				for (size_t j = i; j < cell.dynamicList.size(); ++j)
				{
					CCollisionCom*& coll_2 = cell.dynamicList[j];

					if (coll_1->GetOwner() == coll_2->GetOwner()) continue;
					COLLISION_TYPE c1 = coll_1->GetType();
					COLLISION_TYPE c2 = coll_2->GetType();
					const BLOCKING_TYPE eType1To2 = s_BlockingTypeMatrix[c1][c2];
					const BLOCKING_TYPE eType2To1 = s_BlockingTypeMatrix[c2][c1];
					if (eType1To2 == BLOCKING_END && eType2To1 == BLOCKING_END)
						continue;

					if (IsCollided(
						coll_1->GetTransform()->m_vInfo[INFO_POS], coll_1->GetRadius(),
						coll_2->GetTransform()->m_vInfo[INFO_POS], coll_2->GetRadius()))
					{
						coll_1->CollisionDynamic(coll_2, eType1To2);
						coll_2->CollisionDynamic(coll_1, eType2To1);
					}
				}

				for (auto& coll_static : cell.staticList)
				{
					if (IsCollidedXZ(
						coll_1->GetTransform()->m_vInfo[INFO_POS], coll_1->GetRadius(),
						coll_static.first, coll_static.second))
					{
						coll_1->CollisionStatic(coll_static.first, coll_static.second);
					}
				}
			}
		}
	}
}

void CCollider::Clear_Dynamic()
{
	for(auto& vecRow : m_vecGrid)
	{
		for (auto& cell : vecRow)
		{
			for (auto& collCom : cell.dynamicList)
				Safe_Release(collCom);

			cell.dynamicList.clear();
		}
	}
}

void CCollider::Clear_ColliderAll()
{
	for(auto& vecRow : m_vecGrid)
	{
		for (auto& cell : vecRow)
		{
			cell.dynamicList.clear();
			cell.staticList.clear();
		}
	}
}

void CCollider::Free()
{
	Clear_ColliderAll();
}

bool CCollider::IsCollided(const _vec3& vPos1, _float fRadius1, const _vec3& vPos2, _float fRadius2)
{
	const _vec3 vDiff = vPos1 - vPos2;
	return D3DXVec3LengthSq(&vDiff) <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2);
}

bool CCollider::IsCollidedXZ(const _vec3& vPos1, _float fRadius1, const _vec3& vPos2, _float fRadius2)
{
	const _vec3 vDiff = vPos1 - vPos2;
	const _vec2 vXZDiff{vDiff.x, vDiff.z};
	return D3DXVec2LengthSq(&vXZDiff) <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2);
}

bool CCollider::IsCollided_AABB(const _vec3& vPos1, _float fRadius1, const _vec3& vPos2, _float fRadius2)
{
	const _vec3 vMax1 = vPos1 + _vec3{fRadius1, fRadius1, fRadius1};
	const _vec3 vMin1 = vPos1 - _vec3{fRadius1, fRadius1, fRadius1};

	const _vec3 vMax2 = vPos2 + _vec3{fRadius2, fRadius2, fRadius2};
	const _vec3 vMin2 = vPos2 - _vec3{fRadius2, fRadius2, fRadius2};

	if (vMax1.x < vMin2.x || vMin1.x > vMax2.x) return false;
	if (vMax1.z < vMin2.z || vMin1.z > vMax2.z) return false;
	return true;
}	
