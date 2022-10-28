#include "stdafx.h"
#include "..\Header\Dynamite.h"
#include "Player.h"
#include "StatComponent.h"
#include "TerrainCubeMap.h"
#include "Monster.h"
#include "Particle.h"
#include "AbstFactory.h"
#include "StaticCamera.h"

CDynamite::CDynamite(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}


CDynamite::~CDynamite()
{
}

HRESULT CDynamite::Ready_Object()
{
	m_pTransCom = Add_Component<Engine::CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);
	m_pBufferCom = Add_Component<Engine::CCubeTex>(L"Proto_CubeTexCom", L"Proto_CubeTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<Engine::CTexture>(L"Proto_MinecraftCubeTexture", L"Proto_MinecraftCubeTexture", ID_STATIC);
	m_pShaderCom = Add_Component<Engine::CShader>(L"Proto_DynamiteShaderCom", L"Proto_DynamiteShaderCom", ID_DYNAMIC);
	m_pTransCom->m_vInfo[INFO_POS] = { 3.f, 7.5f, 4.f };

	m_pColl = Add_Component<CCollisionCom>(L"Proto_CollisionCom", L"Proto_CollisionCom", ID_DYNAMIC);
	m_pColl->SetOwner(this);
	m_pColl->SetOwnerTransform(m_pTransCom);
	m_pColl->SetCollOffset(_vec3{ 0.f, 1.5f, 0.f });
	m_pColl->SetRadius(1.f);
	m_pColl->SetCollType(COLL_PLAYER);

	return S_OK;
}

_int CDynamite::Update_Object(const _float & fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	//for shader
	if (m_eState == DYNAMITE_BOOM)
	{
		if (m_fCurFireShaderCount > m_fFireShaderCount)
		{
			m_bTwinkle = !m_bTwinkle;
			m_fCurFireShaderCount = 0.f;
			m_fFreq *= 1.2f;
		}
		else
		{
			m_fCurFireShaderCount += fTimeDelta * m_fFreq;
		}
	}

	switch (m_eState)
	{
	case DYNAMITE_PICK:
	{
		CPlayer* pPlayer = Get_GameObject<CPlayer>(LAYER_PLAYER, L"Player");
		_vec3 vPlayerPos = pPlayer->GetInfo(INFO_POS);

		m_pTransCom->Set_Pos(vPlayerPos.x, vPlayerPos.y + 3.5f, vPlayerPos.z);
	}
		break;
	case DYNAMITE_THROW:
	{
		_vec3 vOrigin, vRayDir, vLookAt;
		_matrix matView, matProj;
		D3DVIEWPORT9 ViewPort;

		ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
		m_pGraphicDev->GetViewport(&ViewPort);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

		if(!m_bRotate)
			RotateToCursor();

		Parabola(fTimeDelta);
	}
		break;
	case DYNAMITE_BOOM:
	{
		m_fTime += fTimeDelta;
		if (m_fTime < 2.f)
			break;

		m_bExplosion = true;
		
	}
		break;
	case DYNAMITE_END:	
		break;
	default:
		_CRASH("wrong access");
	}

	_int iResult = Engine::CGameObject::Update_Object(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	return OBJ_NOEVENT;
}

void CDynamite::LateUpdate_Object()
{

	if (m_bExplosion)
	{
		Get_GameObject<CStaticCamera>(LAYER_ENV, L"StaticCamera")
			->PlayShake(0.2f, 0.8f);
		set<CGameObject*> objSet;
		Engine::GetOverlappedObject(OUT objSet, m_pTransCom->m_vInfo[INFO_POS], 5.f);
		for (auto& obj : objSet)
		{

			if (obj->Has_Component(L"Proto_StatCom", ID_DYNAMIC))
			{
				DamageType eDT = DT_KNOCK_BACK;
				obj->Get_Component<CStatComponent>(L"Proto_StatCom", ID_DYNAMIC)
					->TakeDamage(300, m_pTransCom->m_vInfo[INFO_POS], this, eDT);
			}
		}

		DEBUG_SPHERE(m_pTransCom->m_vInfo[INFO_POS], 5.f, 1.f);

		Get_GameObject<CFireWork>(LAYER_EFFECT, L"FireWork")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 3.f, D3DXCOLOR(1.f, 1.f, 0.2f, 0), 256, 0.4f);
		CEffectFactory::Create<CUVCircle>("Creeper_Explosion", L"Creeper_Explosion", m_pTransCom->m_vInfo[INFO_POS]);
		Get_GameObject<CAttack_P>(LAYER_EFFECT, L"Attack_Basic")->Add_Particle(m_pTransCom->m_vInfo[INFO_POS], 0.3f, RED, 30, 0.5f);
		for (int i = 0; i < 5; i++)
		{
			CEffectFactory::Create<CCloud>("Creeper_Cloud", L"Creeper_Cloud", m_pTransCom->m_vInfo[INFO_POS]);
		}

		CSoundMgr::GetInstance()->PlaySoundRandom({
			L"twinblast_grenade_explosion_01.ogg",
			L"twinblast_grenade_explosion_02.ogg" ,
			L"twinblast_grenade_explosion_03.ogg" },
			Get_GameObject<CPlayer>(LAYER_PLAYER, L"Player")->GetInfo(INFO_POS), 1.f);

		m_bDead = true;
	}
	
	CGameObject::LateUpdate_Object();
}

void CDynamite::Render_Object()
{
	if (!m_bTwinkle)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
		m_pTextureCom->Set_Texture(59);
		m_pBufferCom->Render_Buffer();
	}
	else
	{
		_matrix		WorldMtrix, ViewMatrix, ProjMaatrix;

		m_pTransCom->Get_WorldMatrix(&WorldMtrix);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMaatrix);

		m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMtrix, &WorldMtrix), sizeof(_matrix));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMaatrix, &ProjMaatrix), sizeof(_matrix));
		m_pShaderCom->Set_Bool("g_Countdown", m_bTwinkle);
		m_pTextureCom->Set_Texture(m_pShaderCom, "g_DefaultTexture", 59);

		m_pShaderCom->Begin_Shader(0);
		m_pBufferCom->Render_Buffer();
		m_pShaderCom->End_Shader();
	}
	
}

void CDynamite::Free()
{
	CGameObject::Free();
}

CDynamite * CDynamite::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamite* pInstance = new CDynamite(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CDynamite::RotateToCursor()
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_vec3 vPoint;
	_vec3 vAt;

	D3DVIEWPORT9 ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// ºäÆ÷Æ® -> Åõ¿µ
	vPoint.x = (_float)ptMouse.x / ((_float)ViewPort.Width * 0.5f) - 1.f;
	vPoint.y = (_float)ptMouse.y / -((_float)ViewPort.Height * 0.5f) + 1.f;
	vPoint.z = 0.f;
	vAt.x = vPoint.x;
	vAt.y = vPoint.y;
	vAt.z = 1.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matProj);
	D3DXVec3TransformCoord(&vAt, &vAt, &matProj);

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXVec3TransformCoord(&vPoint, &vPoint, &matView);
	D3DXVec3TransformCoord(&vAt, &vAt, &matView);

	D3DXPLANE tmpPlane;
	D3DXPlaneFromPointNormal(&tmpPlane, &m_pTransCom->m_vInfo[INFO_POS], &CGameUtilMgr::s_vUp);

	_vec3 vLookAt;
	D3DXPlaneIntersectLine(&vLookAt, &tmpPlane, &vPoint, &vAt);

	_vec3 vLook = vLookAt - m_pTransCom->m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vLook, &vLook);

	const _vec2 v2Look{ 0.f, 1.f };
	_vec2 v2ToDest{ vLook.x, vLook.z };

	const _float fDot = D3DXVec2Dot(&v2Look, &v2ToDest);

	if (vLook.x < 0)
		m_pTransCom->m_vAngle.y = -acosf(fDot);
	else
		m_pTransCom->m_vAngle.y = acosf(fDot);

	m_pTransCom->Update_Component(0.f);

	m_bRotate = true;
}

void CDynamite::Parabola(const _float & fTimeDelta)
{
	_float fHeight = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap")->GetHeight(m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].z);

	switch (m_eBounce)
	{
	case THROWING:
	{
		if (m_pTransCom->m_vInfo[INFO_POS].y <= fHeight + 2.5f)
		{
			m_fTime = 0.f;
			m_eBounce = ONE_BOUNCE;
		}
		else
		{
			D3DXVec3Normalize(&m_pTransCom->m_vInfo[INFO_LOOK], &m_pTransCom->m_vInfo[INFO_LOOK]);
			m_pTransCom->m_vInfo[INFO_POS].y += m_fPower * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
			m_pTransCom->m_vInfo[INFO_POS].x += m_fPower * m_pTransCom->m_vInfo[INFO_LOOK].x * fTimeDelta;
			m_pTransCom->m_vInfo[INFO_POS].z += m_fPower * m_pTransCom->m_vInfo[INFO_LOOK].z * fTimeDelta;
			m_fTime += 8.f * fTimeDelta;
		}

	}
		break;
	case ONE_BOUNCE:
	{
		if (m_pTransCom->m_vInfo[INFO_POS].y <= fHeight + 1.5f)
		{
			m_fTime = 0.f;
			m_eBounce = TWO_BOUNCE;
		}
		else
		{
			m_pTransCom->Set_StopUpdate();

			_vec3 vAxis;
			_matrix matRot , matTrans, matWorld;
			D3DXMatrixIdentity(&matWorld);
			D3DXVec3Cross(&vAxis, &m_pTransCom->m_vInfo[INFO_LOOK], &m_pTransCom->m_vInfo[INFO_POS]);
			D3DXMatrixRotationAxis(&matRot, &vAxis,m_fAngle);

			m_pTransCom->m_vInfo[INFO_POS].y += m_fPower * 0.7f * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
			m_pTransCom->m_vInfo[INFO_POS].x += m_fPower * 0.5f * m_pTransCom->m_vInfo[INFO_LOOK].x * fTimeDelta;
			m_pTransCom->m_vInfo[INFO_POS].z += m_fPower * 0.5f * m_pTransCom->m_vInfo[INFO_LOOK].z * fTimeDelta;
			m_fTime += 7.f * fTimeDelta;

			D3DXMatrixTranslation(&matTrans, m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].y, m_pTransCom->m_vInfo[INFO_POS].z);
			matWorld = matRot * matTrans;
			m_pTransCom->Set_WorldMatrix(&matWorld);
			m_pTransCom->Update_Component(fTimeDelta);
		}
	}
		break;
	case TWO_BOUNCE:
	{
		if (m_pTransCom->m_vInfo[INFO_POS].y <= fHeight + 1.f)
		{
			m_fTime = 0.f;
			m_eBounce = BOUNCE_END;
		}
		else
		{
			_vec3 vAxis;
			_matrix matRot, matTrans, matWorld;
			D3DXMatrixIdentity(&matWorld);
			D3DXVec3Cross(&vAxis, &m_pTransCom->m_vInfo[INFO_LOOK], &m_pTransCom->m_vInfo[INFO_POS]);
			D3DXMatrixRotationAxis(&matRot, &vAxis, m_fAngle);

			m_pTransCom->m_vInfo[INFO_POS].y += m_fPower * 0.4f * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
			m_pTransCom->m_vInfo[INFO_POS].x += m_fPower * 0.4f * m_pTransCom->m_vInfo[INFO_LOOK].x * fTimeDelta;
			m_pTransCom->m_vInfo[INFO_POS].z += m_fPower * 0.4f * m_pTransCom->m_vInfo[INFO_LOOK].z * fTimeDelta;
			m_fTime += 6.f * fTimeDelta;

			D3DXMatrixTranslation(&matTrans, m_pTransCom->m_vInfo[INFO_POS].x, m_pTransCom->m_vInfo[INFO_POS].y, m_pTransCom->m_vInfo[INFO_POS].z);
			matWorld = matRot * matTrans;
			m_pTransCom->Set_WorldMatrix(&matWorld);
			m_pTransCom->Update_Component(fTimeDelta);
		}
	}
		break;
	case BOUNCE_END:
		m_pTransCom->Set_StartUpdate();
		m_eState = DYNAMITE_BOOM;
		break;
	default:
		break;
	}

	m_fAngle += 10.f * fTimeDelta;
}