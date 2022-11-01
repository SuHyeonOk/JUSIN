#include "stdafx.h"
#include "..\Header\Item.h"


CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
	, m_fPower(0.f)
	, m_fTime(0.f)
	, m_bIdle(false)
{
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_Object()
{
	return S_OK;
}

_int CItem::Update_Object(const _float & fTimeDelta)
{
	_int iResult = Engine::CGameObject::Update_Object(fTimeDelta);

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

	

	return 0;
}

void CItem::LateUpdate_Object()
{
	Engine::CGameObject::LateUpdate_Object();
}

void CItem::Render_Object()
{
}


void CItem::Free()
{
	CGameObject::Free();
}

void CItem::Parabola(_vec3 & vPos,const _float& fHeight,const _float& fTimeDelta)
{

	if (vPos.y <= fHeight || m_bIdle)
	{
		if (!m_bIdle)
		{
			m_fTime = 0.5f;
			vPos.y = fHeight + 1.f;
		}
		

		if (vPos.y <= (1.5f + fHeight) && vPos.y >= (1.f + fHeight))
		{
			vPos.y += m_fTime * fTimeDelta;
		}
		else
		{
			if (vPos.y >= 1.25f + fHeight)
				vPos.y = 1.5f + fHeight;
			else
				vPos.y = 1.f + fHeight;

			m_fTime *= -1.f;
		}
		
		m_bIdle = true;
	}
	else
	{
		vPos.y += m_fPower * m_fTime * fTimeDelta - (9.8f * m_fTime * m_fTime * fTimeDelta * 0.5f);
		vPos.x += 5.f * m_vVelocity.x * fTimeDelta;
		vPos.z += 5.f * m_vVelocity.z * fTimeDelta;
		m_fTime += 8.f * fTimeDelta;
	}
}
