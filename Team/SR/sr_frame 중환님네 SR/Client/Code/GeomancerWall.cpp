#include "stdafx.h"
#include "GeomancerWall.h"

#include "AbstFactory.h"
#include "Particle.h"
#include "TerrainCubeMap.h"
#include "Player.h"
#include "StatComponent.h"
#include "StaticCamera.h"

CGeomancerWall::CGeomancerWall(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
}

CGeomancerWall::CGeomancerWall(const CGeomancerWall& rhs): CGameObject(rhs)
{
}

CGeomancerWall::~CGeomancerWall()
{
}

HRESULT CGeomancerWall::Ready_Object()
{
	CGameObject::Ready_Object();

	m_pTransform = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pTexture = Add_Component<CTexture>(L"Proto_MinecraftCubeTexture", L"Proto_MinecraftCubeTexture", ID_STATIC);
	m_pBuffer = Add_Component<CCubeTex>(L"Proto_CubeTexCom", L"Proto_CubeTexCom", ID_STATIC);
	m_pShaderCom = Add_Component<CShader>(L"Proto_ColumnShaderCom", L"Proto_ColumnShaderCom", ID_DYNAMIC);

	CCollisionCom* m_pCollCom = Add_Component<CCollisionCom>(L"Proto_CollisionCom", L"Proto_CollisionCom", ID_DYNAMIC);
	m_pCollCom->SetOwner(this);
	m_pCollCom->SetOwnerTransform(m_pTransform);
	m_pCollCom->SetRadius(1.5f);
	m_pCollCom->SetCollOffset(_vec3{0.f, 0.f, 0.f});
	m_pCollCom->SetCollType(COLL_WALL);

	m_fLifeTime += (_float)(rand() % 100) / 100.f;

	m_fLifeTime = m_bBomb ? 4.f : 6.f;
	m_fSpeed = 7.f;

	return S_OK;
}

_int CGeomancerWall::Update_Object(const _float& fTimeDelta)
{
	if (m_bDead) return OBJ_DEAD;

	CGameObject::Update_Object(fTimeDelta);

	if (m_bSetY == false)
	{
		CTerrainCubeMap* pCubeMap = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");

		_vec3& vPos = m_pTransform->m_vInfo[INFO_POS];
		if (vPos.x < 0.f || vPos.x >= VTXCNTX || vPos.z < 0.f || vPos.z >= VTXCNTZ) // 필드 범위 밖
			return OBJ_DEAD;
		m_fFinishY = pCubeMap->GetHeight(vPos.x, vPos.z);

		// 맵 밖에 생성되서 생성 취소
		if (CGameUtilMgr::FloatCmp(m_fFinishY, 0.f))
			return OBJ_DEAD;

		m_fFinishY += 1.5f;
		m_fInitY = m_fFinishY - 1.5f - ((_float)(rand() % 30) / 10.f);
		vPos.y = m_fInitY;

		m_pTransform->m_vScale = {0.5f, 2.5f, 0.5f};
		m_pTransform->Update_Component(0);

		m_bSetY = true;
	}


	_vec3& vPos = m_pTransform->m_vInfo[INFO_POS];

	if (m_fLifeTime < 0.f)
	{
		if (m_bBomb)
		{
			m_bFire = true;
		}
		else
		{
			// 내려가기
			vPos.y -= fTimeDelta * m_fSpeed;
			if (vPos.y < m_fInitY)
				return OBJ_DEAD;
		}
	}
	else
	{
		// 올라오기
		if (m_fFinishY > vPos.y)
		{
			vPos.y += fTimeDelta * m_fSpeed;
		}
	}


	// 수명 줄이기 코드
	m_fLifeTime -= fTimeDelta;

	Add_RenderGroup(RENDER_NONALPHA, this);
	return OBJ_NOEVENT;
}

void CGeomancerWall::LateUpdate_Object()
{
	if (m_bFire)
	{
		// shake
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.15f, 0.4f);

		_vec3& vPos = m_pTransform->m_vInfo[INFO_POS];

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
		
		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 0.3f, D3DXCOLOR(1.f, 0.4f, 0.25f, 0), 256, 0.4f);
		CEffectFactory::Create<CUVCircle>("Creeper_Explosion", L"Creeper_Explosion", m_pTransform->m_vInfo[INFO_POS]);
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransform->m_vInfo[INFO_POS], 0.3f, BLACK, 30, 0.5f);
		for (int i = 0; i < 5; i++)
		{
			CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pTransform->m_vInfo[INFO_POS]);
		}

		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"twinblast_grenade_explosion_01.ogg",
			L"twinblast_grenade_explosion_02.ogg" ,
			L"twinblast_grenade_explosion_03.ogg" },
			m_pTransform->m_vInfo[INFO_POS]);

		DEBUG_SPHERE(vPos, 3.5f, 1.f);
		m_bDead = true;
		m_bFire = false;
	}

}

void CGeomancerWall::Render_Object()
{
	if (!m_bBomb)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->m_matWorld);
		m_pTexture->Set_Texture(m_iTexNum);
		m_pBuffer->Render_Buffer();
	}
	else
	{
		_matrix		WorldMtrix, ViewMatrix, ProjMaatrix;

		m_pTransform->Get_WorldMatrix(&WorldMtrix);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMaatrix);

		m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMtrix, &WorldMtrix), sizeof(_matrix));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMaatrix, &ProjMaatrix), sizeof(_matrix));

		m_pTexture->Set_Texture(m_pShaderCom, "g_DefaultTexture", m_iTexNum);
		m_pShaderCom->Set_RawValue("g_Time", &m_fLifeTime, sizeof(_float));

		m_pShaderCom->Begin_Shader(0);
		m_pBuffer->Render_Buffer();
		m_pShaderCom->End_Shader();
	}
	
}

CGeomancerWall* CGeomancerWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bBomb)
{
	CGeomancerWall* pInst = new CGeomancerWall(pGraphicDev);
	pInst->m_bBomb = bBomb;
	pInst->m_iTexNum = bBomb ? 154 : 153;

	if (FAILED(pInst->Ready_Object()))
	{
		MSG_BOX("Fail to Create Geomancer wall");
		return nullptr;
	}
	return pInst;
}
