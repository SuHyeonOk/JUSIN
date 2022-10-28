#include "stdafx.h"
#include "..\Header\RedStoneMonstrosityBullet.h"
#include "Transform.h"
#include "AbstFactory.h"
#include "TerrainCubeMap.h"
#include "RedStoneMonstrosity.h"
#include "Player.h"
#include "StatComponent.h"
#include "Particle.h"
#include "SphereEffect.h"

CRedStoneMonstrosityBullet::CRedStoneMonstrosityBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CRedStoneMonstrosityBullet::~CRedStoneMonstrosityBullet()
{
}

HRESULT CRedStoneMonstrosityBullet::Ready_Object()
{
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<CRcShader>(L"Proto_SpitCom", L"Proto_SpitCom", ID_STATIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_Spit", L"Proto_Spit", ID_STATIC);
	m_pBufferCom->Set_Texture(m_pTexture->GetDXTexture(0));


	return S_OK;
}

_int CRedStoneMonstrosityBullet::Update_Object(const _float & fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	if (m_bDead)
		return OBJ_DEAD;
	//플레이어 위치로 포물선으로 날아가게 함
	m_pTransCom->m_vInfo[INFO_POS].y += m_fPower * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
	m_pTransCom->m_vInfo[INFO_POS].x += 13.f * m_vDist.x * fTimeDelta;
	m_pTransCom->m_vInfo[INFO_POS].z += 13.f * m_vDist.z * fTimeDelta;
	m_fTime += 5.f * fTimeDelta;

	_matrix		matWorld, matView, matBill;
	D3DXMatrixIdentity(&matBill);

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 현재 지금 이 코드는 문제가 없지만 나중에 문제가 될 수 있음
	m_pTransCom->Set_WorldMatrix(&(matBill * matWorld));

	m_pBufferCom->m_matWorld = m_pTransCom->m_matWorld;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CRedStoneMonstrosityBullet::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
	//바닥에 부딫히면 터지면 사망.

	CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
	_vec3 vPos = m_pTransCom->m_vInfo[INFO_POS];
	
	if (pCubeMap->GetHeight(vPos.x, vPos.z) >= vPos.y)
		m_bDead = true;

	if (m_bDead)
	{
		_vec3& vPos = m_pTransCom->m_vInfo[INFO_POS];

		set<CGameObject*> setObj;
		Engine::GetOverlappedObject(setObj, vPos, 3.5f);

		for (auto& obj : setObj)
		{
			if (CPlayer* pPlayer = dynamic_cast<CPlayer*>(obj))
			{
				pPlayer->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(30, vPos, this, DT_KNOCK_BACK);
				IM_LOG("damage");
				break;
			}
		}

		DEBUG_SPHERE(m_pTransCom->m_vInfo[INFO_POS], 3.5f, 1.f);

		// effect
		CEffectFactory::Create<CSphereEffect>("Golem_Melee_M", L"Golem_Melee_M", m_pTransCom->m_vInfo[INFO_POS]);
		CEffectFactory::Create<CSphereEffect>("Golem_Spit_Sphere", L"Golem_Spit_Sphere", m_pTransCom->m_vInfo[INFO_POS]);
		CEffectFactory::Create<CUVCircle>("Creeper_Explosion", L"Creeper_Explosion", m_pTransCom->m_vInfo[INFO_POS]);
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 0.3f, D3DXCOLOR(0.8f, 0.25f, 0.1f, 1.f), 5, 0.5f);
		for (int i = 0; i < 5; i++)
		{
			CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pTransCom->m_vInfo[INFO_POS]);
		}
		// effect

		return;
	}

	DEBUG_SPHERE(m_pTransCom->m_vInfo[INFO_POS], 1.f, 0.1f);

}

void CRedStoneMonstrosityBullet::Render_Object()
{
	m_pBufferCom->Render_Buffer();
}

CRedStoneMonstrosityBullet * CRedStoneMonstrosityBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRedStoneMonstrosityBullet* pInstance = new CRedStoneMonstrosityBullet(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CRedStoneMonstrosityBullet::Free()
{
	CGameObject::Free();
}
