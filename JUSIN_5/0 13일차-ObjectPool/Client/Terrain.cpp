#include "stdafx.h"
#include "Terrain.h"
#include "TextureMgr.h"
#include "Device.h"
#include "TimeMgr.h"

CTerrain::CTerrain()
{
	m_vecTile.reserve(TILEX * TILEY);
	
}

CTerrain::~CTerrain()
{
	Release();
}

HRESULT CTerrain::Initialize(void)
{
	if (FAILED(Load_Tile(L"../Data/Map.dat")))
		return E_FAIL;

	m_wstrObjKey = L"Terrain";
	m_wstrStateKey = L"Tile";

	Ready_Adjacency();
	
	return S_OK;
}

int CTerrain::Update(void)
{

	if (0.f > ::Get_Mouse().x)
		CObj::m_vScroll.x += 300.f  * CTimeMgr::Get_Instance()->Get_TimeDelta();

	if (WINCX < ::Get_Mouse().x)
		CObj::m_vScroll.x -= 300.f  * CTimeMgr::Get_Instance()->Get_TimeDelta();

	if (0.f > ::Get_Mouse().y)
		CObj::m_vScroll.y += 300.f  * CTimeMgr::Get_Instance()->Get_TimeDelta();

	if (WINCY < ::Get_Mouse().y)
		CObj::m_vScroll.y -= 300.f  * CTimeMgr::Get_Instance()->Get_TimeDelta();
		
	return OBJ_NOEVENT;
}

void CTerrain::Late_Update(void)
{
	
}

void CTerrain::Render(void)
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	TCHAR	szBuf[MIN_STR] = L"";

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{

		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			m_vecTile[i]->vPos.x + CObj::m_vScroll.x,
			m_vecTile[i]->vPos.y + CObj::m_vScroll.y,
			m_vecTile[i]->vPos.z);

		matWorld = matScale * matTrans;

		const TEXINFO*		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(m_wstrObjKey.c_str(), m_wstrStateKey.c_str(), m_vecTile[i]->byDrawID);

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

void CTerrain::Release(void)
{
	for_each(m_vecTile.begin(), m_vecTile.end(), Safe_Delete<TILE*>);
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();
}

HRESULT CTerrain::Load_Tile(const TCHAR* pFilePath)
{

	HANDLE	hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	
	DWORD	dwByte = 0;
	TILE*	pTile = nullptr;

	while (true)
	{
		pTile = new TILE;

		ReadFile(hFile, pTile, sizeof(TILE), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(pTile);
			break;
		}

		m_vecTile.push_back(pTile);
	}

	CloseHandle(hFile);
	return S_OK;
}

void CTerrain::Ready_Adjacency(void)
{
	// 현재 타일의 개수만큼 벡터 메모리를 할당
	m_vecAdjacency.resize(m_vecTile.size());

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			int		iIndex = i * TILEX + j;

			if(0 > iIndex || (size_t)iIndex >= m_vecTile.size())
				continue;


			// 이동할 수 있는 타일인가 && 타일의 옵션이 0인가?

			// 좌상단
			if ((0 != i) && (0 != iIndex % (TILEX * 2)))
			{				
				// 홀->짝 20 감소
				if ((0 != i % 2) && (!m_vecTile[iIndex - TILEX]->byOption))
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex - TILEX]);
				
				// 짝 -> 홀 21 감소 
				else if ((0 == i % 2) && (!m_vecTile[iIndex - (TILEX + 1)]->byOption) )
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex - (TILEX + 1)]);
			}

			// 우상단
			if ((0 != i) && ((TILEX * 2 - 1) != iIndex % (TILEX * 2)))
			{
				// 홀->짝 19 감소
				if ((0 != i % 2) && (!m_vecTile[iIndex - (TILEX - 1)]->byOption) )
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex - (TILEX- 1)]);

				// 짝 -> 홀 20 감소 
				else if ((0 == i % 2) && (!m_vecTile[iIndex - TILEX]->byOption) )
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex - TILEX]);
			}

			// 좌하단
			if ((TILEY - 1 != i) && (0 != iIndex % (TILEX * 2)))
			{
				// 홀->짝 20 증가
				if ((0 != i % 2) && (!m_vecTile[iIndex + TILEX]->byOption) )
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex + TILEX]);

				// 짝 -> 홀 19 증가
				else if ((0 == i % 2) && (!m_vecTile[iIndex + (TILEX - 1)]->byOption))
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex + (TILEX - 1)]);
			}

			// 우하단
			if ((TILEY - 1 != i) && ((TILEX * 2 - 1) != iIndex % (TILEX * 2)))
			{
				// 홀->짝 21 증가
				if ((0 != i % 2) && (!m_vecTile[iIndex + (TILEX + 1)]->byOption))
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex + (TILEX + 1)]);

				// 짝 -> 홀 20 증가
				else if ((0 == i % 2) && (!m_vecTile[iIndex + TILEX]->byOption))
					m_vecAdjacency[iIndex].push_back(m_vecTile[iIndex + TILEX]);
			}
		}
	}

}
