#include "stdafx.h"
#include "ItemTexUI.h"


CItemTexUI::CItemTexUI(LPDIRECT3DDEVICE9 pGraphicDev) : CUI(pGraphicDev)
{
}


CItemTexUI::~CItemTexUI()
{
}

HRESULT CItemTexUI::Ready_Object()
{
	D3DXMatrixOrthoLH(&m_ProjMatrix, WINCX, WINCY, 0.f, 1.f);

	m_pBufferCom = Add_Component<CRcTex>(L"Proto_RcTexCom", L"Proto_RcTexCom", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_InventoryUI_Texture", L"Proto_InventoryUI_Texture", ID_STATIC);
	m_pTransCom = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);


	m_iTexNum = 0;
	m_iRenderPriority = 9;
	return S_OK;
}

_int CItemTexUI::Update_Object(const _float & fTimeDelta)
{
	if (m_bClose)
		return OBJ_NOEVENT;

	CUI::Update_Object(fTimeDelta);


	if (m_bWorldSet == false)
	{
		_vec3 vScale = m_pTransCom->m_vScale;
		_vec3 vPos = _vec3(WINCX * 0.37f, WINCY * 0.29f, 0.f);
		_vec3 vAngle = m_pTransCom->m_vAngle;
		vScale.x *= 0.1f;
		vScale.y *= 0.15f;
		CGameUtilMgr::MatWorldComposeEuler(matworld, vScale, vAngle, vPos);

		m_bWorldSet = true;
	}
	
		
	

	return OBJ_NOEVENT;
}

void CItemTexUI::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &CGameUtilMgr::s_matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);



	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
	m_pTextureCom->Set_Texture(m_iTexNum);
	m_pBufferCom->Render_Buffer();

	SetTexture(m_iTexNum);

	_vec3 vPos = { matworld._41, matworld._42, 0.f };

	Render_Font(L"Gothic_Regular30", wstrItemName[0].c_str(),
		&_vec2(vPos.x + WINCX * 0.28f, -vPos.y + WINCY *0.48f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	Render_Font(L"Gothic_Bold20", wstrItemName[1].c_str(),
		&_vec2(vPos.x + WINCX * 0.28f, -vPos.y + WINCY *0.58f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	Render_Font(L"Gothic_Bold20", wstrItemName[2].c_str(),
		&_vec2(vPos.x + WINCX * 0.28f, -vPos.y + WINCY *0.65f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	Render_Font(L"Gothic_Bold20", L"효과 부여",
		&_vec2(vPos.x + WINCX * 0.395f, vPos.y + WINCY *0.45f),
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CItemTexUI::Free()
{
	CUI::Free();
}

CItemTexUI * CItemTexUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iTexNum)
{
	CItemTexUI* pInstance = new CItemTexUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}


void CItemTexUI::SetTexture(_int iTexNum)
{
	switch (iTexNum)
	{
	case 7:
	{
		//axe
		wstrItemName[0] = L"도끼";
		wstrItemName[1] = L"근접 피해 60";
		wstrItemName[2] = L"도끼는 효율적인 무기로,\n안선생 조원들의 정훈 교육에 사용됩니다.";
	}
	break;
	case 8:
	{
		//crossbow
		wstrItemName[0] = L"보우건";
		wstrItemName[1] = L"원거리 피해 20";
		wstrItemName[2] = L"단순하지만 균형이 잡혀 있는 무기입니다.\n조원들은 조장의 화살 충돌처리에\n실망을 했다고 전해집니다.";
	}
	break;
	case 9:
	{
		//galive
		wstrItemName[0] = L"글레이브";
		wstrItemName[1] = L"근거리 피해 35";
		wstrItemName[2] = L"안선생님의 뛰어난 조원들 중 한명에게 시연회 \n정복을 치하하는 의미로 하사된 무기입니다.\n이 글레이브는 코딩의 신이 깃들어 있습니다.";
	}
	break;
	case 10:
	{
		//sword
		wstrItemName[0] = L"검";
		wstrItemName[1] = L"근거리 피해 25";
		wstrItemName[2] = L"튼튼하고 믿고 쓸 수 있는 검입니다.";
	}
	break;
	case 11:
	{
		//boots
		wstrItemName[0] = L"신속의 부츠";
		wstrItemName[1] = L"지속시간 10";
		wstrItemName[2] = L"효과부여를 통해 신속한 움직임을 가능하게\n하는 부츠입니다. 지금처럼 모든 게 불확실한\n때에 유용합니다. ";
	}
	break;
	case 13:
	{
		//firework
		wstrItemName[0] = L"폭죽 화살";
		wstrItemName[1] = L"원거리 피해 50";
		wstrItemName[2] = L"파티클 이뻐서 넣었습니다. 불만 있나요?";

	}
	break;
	case 14:
	{
		//shockpowder
		wstrItemName[0] = L"충격 가루";
		wstrItemName[1] = L"마비 지속시간 2초";
		wstrItemName[2] = L"충격 가루는 빠르게 빠져나가고 싶은 사람들을\n위한 믿음직한 도구입니다.";
	}
	break;
	case 15:
	{
		//Laser
		wstrItemName[0] = L"레이저 룬";
		wstrItemName[1] = L"레이저 유지 시간 2초";
		wstrItemName[2] = L"보우건의 화살이 레이저로 바뀝니다. ";
	}
	break;
	case 16:
	{
		//multishot
		wstrItemName[0] = L"멀티샷";
		wstrItemName[1] = L"화살 5발이 동시에 나간다";
		wstrItemName[2] = L"폭죽 화살과 같이 사용하면 어떤일이 일어날까요?";
	}
	break;
	case 17:
	{
		//썬더
		wstrItemName[0] = L"번개 룬";
		wstrItemName[1] = L"검에 번개 속성 부여";
		wstrItemName[2] = L"검 주위에 스파크가 튀면서 몬스터들을 감전시킵니다.";
	}
	break;
	case 18:
	{
		//토네이도
		wstrItemName[0] = L"회오리 룬";
		wstrItemName[1] = L"글레이브에 바람 속성 부여";
		wstrItemName[2] = L"공격을 맞은 적들은 화상데미지를 입습니다.";
	}
	break;
	case 19:
	{
		//기절 룬
		wstrItemName[0] = L"기절 룬";
		wstrItemName[1] = L"도끼에 기절 효과 부여";
		wstrItemName[2] = L"도끼의 공격범위 안에 있는 몬스터들은 기절에 걸립니다.";
	}
	break;
	case 20:
	{
		//공격력 증가
		wstrItemName[0] = L"공격 룬";
		wstrItemName[1] = L"공격력 증가";
		wstrItemName[2] = L"공격력이 증가합니다.";
	}
	break;
	default:
		MSG_BOX("input nonTextIndex");
		break;
	}
}

