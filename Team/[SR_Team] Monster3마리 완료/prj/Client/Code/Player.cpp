#include "stdafx.h"
#include "..\Header\Player.h"

#include "Export_Function.h"
#include "BulletMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_vDirection(0.f, 0.f, 0.f)
	, m_fAccel(0.01f)
	, m_fJSpeed0(0.3f)
	, m_fJSpeed(0.3f)
	, m_fShakeSpeed(0.02f)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

_int CPlayer::Update_Object(const _float & fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	Mouse_Move();
	Key_Input(fTimeDelta);
	Jump(fTimeDelta);
	
	Engine::CGameObject::Update_Object(fTimeDelta);
	
	//Shake(fTimeDelta);
	
	Add_RenderGroup(RENDER_NONALPHA, this); // TestPlayer를 렌더그룹에 포함
	return 0;
}

void CPlayer::LateUpdate_Object(void)
{
	Mouse_Click(m_fTimeDelta);

	if(!m_bJump)
		Set_OnTerrain();
}

void CPlayer::Render_Obejct(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	// TestPlayer의 버퍼 컴포넌트를 출력 후 원래의 셋팅을 복원해야만 다른 오브젝트에 반영되지 않는다.
	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CPlayer::Add_Component(void)
{
	CComponent*		pComponent = nullptr;

	// 렌더링할 버퍼 컴포넌트
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	// 텍스쳐 컴객체 관리 컴포넌트
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Clone_Proto(L"Proto_PlayerTexture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture", pComponent });

	// 월드행렬 변환 트랜스폼 컴포넌트
	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// 지형타기용 계산 컴포넌트
	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Clone_Proto(L"Proto_CalculatorCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CalculatorCom", pComponent });

	return S_OK;
}

void CPlayer::Key_Input(const _float & fTimeDelta)
{
	m_pTransCom->Get_Info(INFO_LOOK, &m_vDirection);

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)	// 전진
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * m_fSpeed * fTimeDelta));
	}

	if (Engine::Get_DIKeyState(DIK_S) & 0x80)	// 후진
	{
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * -m_fSpeed * fTimeDelta));
	}

	if (Engine::Get_DIKeyState(DIK_A) & 0x80)	// 왼쪽 게걸음
	{
		m_pTransCom->Get_Info(INFO_RIGHT, &m_vDirection);

		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * -m_fSpeed * fTimeDelta));
	}

	if (Engine::Get_DIKeyState(DIK_D) & 0x80)	// 오른쪽 게걸음
	{
		m_pTransCom->Get_Info(INFO_RIGHT, &m_vDirection);
		D3DXVec3Normalize(&m_vDirection, &m_vDirection);
		m_pTransCom->Move_Pos(&(m_vDirection * m_fSpeed * fTimeDelta));
	}
	
	if (Engine::Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		if(!m_bJump)
			m_bJump = true;
	}

	if (Engine::Get_DIKeyState(DIK_B) & 0x80)
	{
		if (!m_bShake)
			m_bShake = true;
	}

}

void CPlayer::Mouse_Move(void)
{
	_long	dwMouseMove = 0;

	// 좌우회전은 대부분 y축기준으로 회전
	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		m_pTransCom->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		m_pTransCom->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 10.f));

		//_vec3	vRight;
		//m_pTransCom->Get_Info(INFO_RIGHT, &vRight);
		//D3DXVec3Normalize(&vRight, &vRight);

		//_matrix matRot;
		//D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		//_matrix matWorld = *m_pTransCom->Get_WorldMatrixPointer();
		//matWorld = matRot * matWorld;
		//m_pTransCom->Set_WorldMatrix(matWorld);

		//m_pTransCom->Get_Info(INFO_LOOK, &m_vDirection);
	}
}

void CPlayer::Mouse_Click(const _float& fTimeDelta)
{
	//if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	m_fLBClick += fTimeDelta;

	if((Engine::Get_DIKeyState(DIK_X) & 0x80) && (0.3f<m_fLBClick))
	{
		m_fLBClick = 0.f;
		_vec3 vPos, vLook;
		m_pTransCom->Get_Info(INFO_POS, &vPos);
		m_pTransCom->Get_Info(INFO_LOOK, &vLook);

		CBulletMgr::GetInstance()->Fire(BULLET_WAND);
		//CBulletMgr::GetInstance()->Reuse_Obj(vPos, vLook);
	}
}

void CPlayer::Set_OnTerrain(void)
{
	//m_bJump = false;

	_vec3		vPos;
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	//Engine::CTerrainTex*	pTerrainTexCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Layer_Environment", L"Terrain", L"Proto_TerrainTexCom", ID_STATIC));
	//NULL_CHECK(pTerrainTexCom);

	//_float fHeight = m_pCalculatorCom->HeightOnTerrain(&vPos, pTerrainTexCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);

	_float fHeight = Get_Height();

	m_pTransCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

void CPlayer::Jump(const _float & fTimeDelta)
{
	if (m_bJump)
	{
		_vec3 vPos;
		m_pTransCom->Get_Info(INFO_POS, &vPos);


		/*Engine::CTerrainTex*	pTerrainTexCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Layer_Environment", L"Terrain", L"Proto_TerrainTexCom", ID_STATIC));
		NULL_CHECK(pTerrainTexCom);

		_float fHeight = m_pCalculatorCom->HeightOnTerrain(&vPos, pTerrainTexCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);*/
		_float fHeight = Get_Height();

		if (m_fJTimeDelta > 0.3f && fHeight >= vPos.y)
		{
			m_bJump = false;
			m_fJTimeDelta = 0.f;
			//vPos.y = fHeight;
			m_pTransCom->Set_Pos(vPos.x, fHeight, vPos.z);
			m_fJSpeed = m_fJSpeed0;
		}
		else
		{
			//_float fDist = fHeight + m_fJSpeed0*m_fJTimeDelta - 0.5 * m_fAccel * m_fJTimeDelta * m_fJTimeDelta;
			//_float fVelocity = m_fJSpeed0 - m_fAccel;
			m_fJSpeed -= m_fAccel;
			m_pTransCom->Plus_PosY(m_fJSpeed);
			m_fJTimeDelta += 0.1f;
		}
	}
}

_float CPlayer::Get_Height()
{
	_vec3 vPos;
	m_pTransCom->Get_Info(INFO_POS, &vPos);

	Engine::CTerrainTex*	pTerrainTexCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Layer_Environment", L"Terrain", L"Proto_TerrainTexCom", ID_STATIC));
	NULL_CHECK_RETURN(pTerrainTexCom, 0.f);

	return m_pCalculatorCom->HeightOnTerrain(&vPos, pTerrainTexCom->Get_VtxPos(), VTXCNTX, VTXCNTZ) + 1 * m_fScale;
}

void CPlayer::Shake(const _float & fTimeDelta)
{
	if (!m_bShake)
		return;

	_float fPosY, fPosYd = 0.f;
	_vec3 vPos;
	m_pTransCom->Get_Info(INFO_POS, &vPos);
	fPosY = vPos.y;

	//if (0.1f < m_fShakeTimeAcc)
	//{
	//	_vec3 vPos;
	//	m_pTransCom->Get_Info(INFO_POS, &vPos);
	//	m_fPosY = vPos.y;

	//	vPos.y += 0.2;
	//	m_pTransCom->Set_Y(vPos.y + 1.2f);
	//}
	//if (0.12f < m_fShakeTimeAcc)
	//{
	//	//m_fPosY -= 0.2;
	//	m_fPosY *= 1;
	//	m_pTransCom->Set_Y(m_fPosY );
	//	m_fShakeTimeAcc = 0.f;
	//}
	
	
	m_fShakeTimeAcc += fTimeDelta;
	m_pTransCom->Set_Y(m_fPosY + m_fShakeSpeed);
	if (0.1f < m_fShakeTimeAcc)
	{
		m_fShakeSpeed *= -1.f;
		m_fShakeTimeAcc = 0.f;
	}

	m_fNoShakeTimeAcc += fTimeDelta;
	if (0.5f < m_fNoShakeTimeAcc)
	{
		m_bShake = false;
		m_fNoShakeTimeAcc = 0.f;
	}
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer *	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free(void)
{
	CGameObject::Free();
}
