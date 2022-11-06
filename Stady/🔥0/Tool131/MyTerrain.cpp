#include "stdafx.h"
#include "MyTerrain.h"
#include "TextureMgr.h"
#include "Device.h"
#include "Tool131View.h"


CMyTerrain::CMyTerrain()
{
	m_vecTile.reserve(TILEX * TILEY);
}


CMyTerrain::~CMyTerrain()
{
	Release();
}

bool CMyTerrain::Picking(const D3DXVECTOR3 & vPos, const int & iIndex)
{
	// y = ax + b

	// 12->3->6->9

	float	fGradient[4] {

		((TILECY / 2.f) / (TILECX / 2.f)) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f),
		((TILECY / 2.f) / (TILECX / 2.f)) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f)

	};

	D3DXVECTOR3	vPoint[4]{

		{m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f},
		{ m_vecTile[iIndex]->vPos.x + (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x - (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f }

	};

	// y = ax + b 
	// y - b = ax 
	// -b = ax - y 
	
	// b = y - ax

	float	fB[4]{

		vPoint[0].y - fGradient[0] * vPoint[0].x, 
		vPoint[1].y - fGradient[1] * vPoint[1].x,
		vPoint[2].y - fGradient[2] * vPoint[2].x,
		vPoint[3].y - fGradient[3] * vPoint[3].x	
	};

	bool	bCheck[4]{ false };

	//0 == ax + b - y : 직선 상에 있음
	//0 > ax + b - y : 직선보다 위
	//0 < ax + b - y : 직선보다 아래

	// 12->3
	if (0 < fGradient[0] * vPos.x + fB[0] - vPos.y)
		bCheck[0] = true;

	// 3 -> 6
	if (0 >= fGradient[1] * vPos.x + fB[1] - vPos.y)
		bCheck[1] = true;

	// 6 -> 9
	if (0 >= fGradient[2] * vPos.x + fB[2] - vPos.y)
		bCheck[2] = true;

	// 9 -> 12
	if (0 < fGradient[3] * vPos.x + fB[3] - vPos.y)
		bCheck[3] = true;

	return bCheck[0] && bCheck[1] && bCheck[2] && bCheck[3];

	
}

bool CMyTerrain::Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex)
{
	D3DXVECTOR3	vPoint[4]{

		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x + (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x - (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f }

	};

	D3DXVECTOR3	vDir[4]{

		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]		
	};

	D3DXVECTOR3	vNormal[4]{

		D3DXVECTOR3(-vDir[0].y , vDir[0].x, 0.f),
		D3DXVECTOR3(-vDir[1].y , vDir[1].x, 0.f),
		D3DXVECTOR3(-vDir[2].y , vDir[2].x, 0.f),
		D3DXVECTOR3(-vDir[3].y , vDir[3].x, 0.f)
	};

	D3DXVECTOR3	vMouseDir[4]{

		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]
	};

	for (int i = 0; i < 4; ++i)
	{
		D3DXVec3Normalize(&vNormal[i], &vNormal[i]);
		D3DXVec3Normalize(&vMouseDir[i], &vMouseDir[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < D3DXVec3Dot(&vNormal[i], &vMouseDir[i]))
			return false;
	}

	return true;
}

int CMyTerrain::Get_TileIndex(const D3DXVECTOR3 & vPos)
{
	for (size_t index = 0; index < m_vecTile.size(); ++index)
	{
		if (Picking_Dot(vPos, index))
		{
			return index;
		}
	}

	return -1;
}

void CMyTerrain::Tile_Change(const D3DXVECTOR3 & vPos, const BYTE & _byDrawID)
{
	int		iIndex = Get_TileIndex(vPos);

	if (-1 == iIndex)
		return;

	m_vecTile[iIndex]->byDrawID = _byDrawID;
	m_vecTile[iIndex]->byOption = 1;
}

void CMyTerrain::Initialize(void)
{
	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(TEX_MULTI, L"../Texture/Stage/Terrain/Tile/Tile%d.png", L"Terrain", L"Tile", 36)))
		return;

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			TILE*	pTile = new TILE;

			float	fX = (TILECX * j) + (i % 2) * (TILECX / 2.f);
			float	fY = (TILECY / 2.f) * i;

			pTile->vPos = { fX , fY , 0.f };
			pTile->vSize = { (float)TILECX, (float)TILECY, 0.f };

			pTile->byOption = 0;
			pTile->byDrawID = 3;

			pTile->iIndex = i * TILEX + j;
			pTile->iParentIndex = 0;

			m_vecTile.push_back(pTile);
		}
	
	}
	
}

void CMyTerrain::Update(void)
{
}

void CMyTerrain::Mini_Render(void)
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	TCHAR	szBuf[MIN_STR] = L"";

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{

		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			m_vecTile[i]->vPos.x,  // x스크롤
			m_vecTile[i]->vPos.y,  // y스크롤
			m_vecTile[i]->vPos.z);

		matWorld = matScale * matTrans;

		Set_Ratio(&matWorld, 0.3f, 0.3f);

		const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Terrain", L"Tile", m_vecTile[i]->byDrawID);

		float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		// Draw : 스크린 공간에 2D 스프라이트 이미지를 출력하는 함수
		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture, // 텍스처 컴객체
			nullptr, // 출력할 이미지 영역에 대한 RECT 정보, NULL인 경우 이미지의 0,0 기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심축에 대한 VECTOR3 구조체 포인터, NULL인 경우 0,0이 중심 좌표
			nullptr, // 위치 좌표에 대한 VECTOR3 구조체, NULL인 경우 스크린 상의 0,0좌표에 출력
			D3DCOLOR_ARGB(255, 255, 255, 255)); // 출력할 이미지와 섞을 색상 값, 0xffffffff를 넣어주면 텍스처의 원본 색상을 유지

	}

}

void CMyTerrain::Render(void)
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	TCHAR	szBuf[MIN_STR] = L"";

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{

		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans, 
			m_vecTile[i]->vPos.x - m_pMainView->GetScrollPos(0),  // x스크롤
			m_vecTile[i]->vPos.y - m_pMainView->GetScrollPos(1),  // y스크롤
			m_vecTile[i]->vPos.z);

		matWorld = matScale * matTrans;

		RECT	rc{};

		::GetClientRect(m_pMainView->m_hWnd, &rc);

		float	fX = WINCX / float(rc.right - rc.left);
		float	fY = WINCY / float(rc.bottom - rc.top);

		Set_Ratio(&matWorld, fX, fY);
				
		const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Terrain", L"Tile", m_vecTile[i]->byDrawID);

		float	fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float	fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		// Draw : 스크린 공간에 2D 스프라이트 이미지를 출력하는 함수
		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture, // 텍스처 컴객체
			nullptr, // 출력할 이미지 영역에 대한 RECT 정보, NULL인 경우 이미지의 0,0 기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심축에 대한 VECTOR3 구조체 포인터, NULL인 경우 0,0이 중심 좌표
			nullptr, // 위치 좌표에 대한 VECTOR3 구조체, NULL인 경우 스크린 상의 0,0좌표에 출력
			D3DCOLOR_ARGB(255, 255, 255, 255)); // 출력할 이미지와 섞을 색상 값, 0xffffffff를 넣어주면 텍스처의 원본 색상을 유지

		swprintf_s(szBuf, L"%d", i);
		CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(), szBuf, lstrlen(szBuf), nullptr, 0, D3DCOLOR_ARGB(255, 0, 0, 0));
	}
}

void CMyTerrain::Release(void)
{
	for_each(m_vecTile.begin(), m_vecTile.end(), CDeleteObj());
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

void CMyTerrain::Set_Ratio(D3DXMATRIX* pOut, float fRatioX, float fRatioY)
{
	pOut->_11 *= fRatioX;
	pOut->_21 *= fRatioX;
	pOut->_31 *= fRatioX;
	pOut->_41 *= fRatioX;

	pOut->_12 *= fRatioY;
	pOut->_22 *= fRatioY;
	pOut->_32 *= fRatioY;
	pOut->_42 *= fRatioY;

}
