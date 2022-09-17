#include "..\..\Header\TerrainTex.h"
#include "Export_Function.h"

USING(Engine)

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev), m_hFile(nullptr)
{
	ZeroMemory(&m_fH, sizeof(BITMAPFILEHEADER));
	ZeroMemory(&m_iH, sizeof(BITMAPINFOHEADER));
}

Engine::CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	: CVIBuffer(rhs)
	, m_hFile(rhs.m_hFile)
{
	memcpy(&m_fH, &rhs.m_fH, sizeof(BITMAPFILEHEADER));
	memcpy(&m_iH, &rhs.m_iH, sizeof(BITMAPINFOHEADER));
}

CTerrainTex::~CTerrainTex()
{
}

HRESULT CTerrainTex::Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
	/*LPDIRECT3DTEXTURE9		pTexture = nullptr;
	// 디바이스, 가로, 세로, 민랩, 정적 or 동적, 알파 사용, 정적 버퍼, 컴 객체 주소
	D3DXCreateTexture(m_pGraphicDev, 129, 129, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);

	D3DLOCKED_RECT		LockRect;

	// 직접 픽셀에 접근하기 위해 Lock, Unlock
	pTexture->LockRect(0, &LockRect, nullptr, 0);
	
	for (_int i = 0; i < 129; ++i)
	{
		for (_int j = 0; j < 129; ++j)
		{
			_int	iIndex = i * 129 + j;
			*(((_ulong*)LockRect.pBits) + iIndex) = D3DCOLOR_ARGB(255, 255, 0, 0); // 빨간색
		}
	}
	pTexture->UnlockRect(0);

	D3DXSaveTextureToFile(L"../Bin/Resource/Texture/Test.bmp", D3DXIFF_BMP, pTexture, nullptr);*/

	
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	// CTerrainTex::Ready_Buffer()
	// 파일 가져오기
	_ulong	dwByte = 0;

	m_hFile = CreateFile(L"../Bin/Resource/Texture/Terrain/Height1.bmp", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NO_SCRUB_DATA, 0);

	// 못 읽어 들여 왔을 때 예외처리 안 하심

	// 파일 계층 읽어 들이기
	ReadFile(m_hFile, &m_fH, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(m_hFile, &m_iH, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*		pPixel = new _ulong[m_iH.biHeight * m_iH.biWidth]; // 전체 픽셀의 개수

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_iH.biHeight * m_iH.biWidth, &dwByte, nullptr); // 읽어 들여오기

	CloseHandle(m_hFile); // 개방 파일 소멸

	_ulong	dwIndex = 0;

	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	// 3인자 : 배열에 저장된 첫 번째 버텍스의 주소를 얻어옴

	// CTerrainTex::Ready_Buffer()
	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPos = { _float(j) * dwVtxItv,
									(pPixel[dwIndex] & 0x000000ff) / 20.f,  
									_float(i) * dwVtxItv };

			pVertex[dwIndex].vTexUV = { _float(j) / (dwCntX - 1) * 20.f, 
										_float(i) / (dwCntZ - 1) * 20.f };
		}
	}
	
	m_pVB->Unlock();

	Safe_Delete_Array(pPixel); 

	_ulong dwTriCnt = 0;

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);


	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			// 오른쪽 위
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;
			++dwTriCnt;

			// 왼쪽 아래
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;
			++dwTriCnt;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

void CTerrainTex::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CTerrainTex * CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
	CTerrainTex*	pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent * CTerrainTex::Clone(void)
{
	return new CTerrainTex(*this);
}

void CTerrainTex::Free(void)
{
	CVIBuffer::Free();
}
