#include "stdafx.h"
#include "..\Header\HPGauge.h"

#include "Export_Function.h"

CHPGauge::CHPGauge(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
	, m_fHp(0.f)
	, m_fMinusHp(0.f)
	, m_fMaxHp(0.f)
	, m_fMinHp(0.f)
	, m_fLBClick(0.f)
{
}

CHPGauge::~CHPGauge()
{
}

HRESULT CHPGauge::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matWorld);

	/////////// ������ ����
	m_fScaleX = 120.f;
	m_fScaleY = 30.f;

	m_fPosX = WINCX / -2.5;
	m_fPosY = WINCY / -2.5;

	// ������ ��
	D3DXMatrixScaling(&m_matView, m_fScaleX, m_fScaleY, 1.f);

	// ������
	m_matView._41 = m_fPosX;
	m_matView._42 = m_fPosY;

	// �� �Է��ϴ� ���ڰ� �������� ������ ��
	m_fMaxHp = 20.f;

	// ������ ������ HP ����
	m_fMinusHp = (m_fScaleX / m_fMaxHp) - 1.f; // -1.f? ���� ��Ʈ ������ 

	return S_OK;
}

_int CHPGauge::Update_Object(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_UI, this);

	m_fLBClick += fTimeDelta;
	if (Engine::Get_DIKeyState(DIK_B) & 0x80 && (0.1f<m_fLBClick))
	{
		if (20.f > m_fScaleX) // �������� �������� �Ѿ�� �ʵ���
			return 0;

		m_fScaleX -= m_fMinusHp;
		m_fPosX -= m_fMinusHp;

		D3DXMatrixScaling(&m_matView, m_fScaleX, m_fScaleY, 1.f);

		m_matView._41 = m_fPosX;
		m_matView._42 = m_fPosY;

		m_fLBClick = 0.f;
	}
	return 0;
}

void CHPGauge::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
}

void CHPGauge::Render_Obejct(void)
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x00);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(0);	// �ؽ�ó ���� ������ �켱������ �Ѵ�.
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

HRESULT CHPGauge::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTexCom"));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTexCom", pComponent });

	pComponent = m_pTransCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_TransformCom"));
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_TransformCom", pComponent });

	// m_pTextureCom	
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UI_HPGauge_Texture"));
	NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_UI_HPGauge_Texture", pComponent });
	
	return S_OK;
}

CHPGauge * CHPGauge::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHPGauge *	pInstance = new CHPGauge(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CHPGauge::Free(void)
{
	CUI::Free();
}
