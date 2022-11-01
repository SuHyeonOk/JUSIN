#include "stdafx.h"
#include "..\Header\TerrainCubeMap.h"
#include <set>


CTerrainCubeMap::CTerrainCubeMap(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
	ZeroMemory(&m_fHeight, sizeof(_float) * VTXCNTX * VTXCNTZ);
	
	ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));

	m_Material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_Material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_Material.Ambient = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.f);
	m_Material.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	m_Material.Power = 0.f;
}

CTerrainCubeMap::~CTerrainCubeMap()
{
}


HRESULT CTerrainCubeMap::Ready_Object(const wstring& wstrPath)
{
	m_pCubeTextureCom = Add_Component<CTexture>(L"Proto_MinecraftCubeTexture", L"Proto_MinecraftCubeTexture", ID_STATIC);
	m_pTextureCom = Add_Component<CTexture>(L"Proto_PlantTexture", L"Proto_PlantTexture", ID_STATIC);

	LoadMap(wstrPath);
	 

	return S_OK;
}

_int CTerrainCubeMap::Update_Object(const _float & fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_PRIORITY, this);
	return 0;
}

void CTerrainCubeMap::LateUpdate_Object(void)
{
	Engine::CGameObject::LateUpdate_Object();
	// CTerrainCubeMap* tmp = Get_GameObject<CTerrainCubeMap>(LAYER_ENV, L"TerrainCubeMap");
}

void CTerrainCubeMap::Render_Object(void)
{
	vector<pair<_int, CTerrainCubeTex*>> vecAlphaTex;
	vector<pair<_int, CTerrainCubeTex*>> vecNonAlphaTex;

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	

	for (auto& cubeTex : m_mapTerrainCubeCom)
	{
		if (cubeTex.second->GetCubeCnt() > 0)
		{
			if (cubeTex.first != 60)
				vecAlphaTex.push_back(cubeTex);
			else
				vecNonAlphaTex.push_back(cubeTex);
		}
	}



	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetMaterial(&m_Material);

	for (auto& cubeTex : vecNonAlphaTex)
	{
		m_pCubeTextureCom->Set_Texture(cubeTex.first);
		cubeTex.second->Render_Buffer();
	}



	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);


	
	for (auto& cubeTex : vecAlphaTex)
	{
		m_pCubeTextureCom->Set_Texture(cubeTex.first);
		cubeTex.second->Render_Buffer();
	}

	
	for (auto& cubeTex : m_mapTerrainRcCom)
	{
		if (cubeTex.second->GetTexCnt() > 0)
		{		

			m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_pTextureCom->Set_Texture(cubeTex.first);
			cubeTex.second->Render_Buffer();
		}
	}


	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

}

void CTerrainCubeMap::LoadMap(const wstring& wstrPath)
{
	m_vecTotalCube.clear();
	m_vecLand.clear();
	m_vecCollision.clear();
	m_vecTotalTex.clear();


	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed Load Map");
		return;
	}
	DWORD	dwByte = 0;

	size_t vecCubeSize;
	size_t vecTexSize;
	MapCubeInfo tMapCubeInfo;

	ReadFile(hFile, &vecCubeSize, sizeof(size_t), &dwByte, nullptr);
	ReadFile(hFile, &vecTexSize, sizeof(size_t), &dwByte, nullptr);

	for (size_t i = 0; i < vecCubeSize; ++i)
	{
		ReadFile(hFile, &tMapCubeInfo, sizeof(MapCubeInfo), &dwByte, nullptr);

		m_vecTotalCube.push_back(tMapCubeInfo);

		//divide cubetype
		switch (tMapCubeInfo.eType)
		{
		case TYPE_LAND:
			m_vecLand.push_back(tMapCubeInfo);
			break;
		case TYPE_COLLISION:
			m_vecCollision.push_back(tMapCubeInfo);
			break;
		case TYPE_DECO:
			break;
		default:
			MSG_BOX("Invalid MapCubeInfo when load CTerrainCubeMap");
			break;
		}
	}

	for (size_t i = 0; i < vecTexSize; ++i)
	{
		ReadFile(hFile, &tMapCubeInfo, sizeof(MapCubeInfo), &dwByte, nullptr);
		m_vecTotalTex.push_back(tMapCubeInfo);
	}

	ReadFile(hFile, &m_fHeight, sizeof(m_fHeight), &dwByte, nullptr);

	CloseHandle(hFile);


	set<_int> Cubeidx;
	for (auto i : m_vecTotalCube)
		Cubeidx.insert(i.iTexIdx);

	for (auto texidx : Cubeidx)
	{
		vector<_matrix> vecmatWorld;

		for (auto i : m_vecTotalCube)
		{
			if (texidx == i.iTexIdx)
				vecmatWorld.push_back(i.matWorld);
		}


		auto iter = m_mapTerrainCubeCom.find(texidx);
		if (iter == m_mapTerrainCubeCom.end())
		{
			CTerrainCubeTex* pTerrainTex = CTerrainCubeTex::Create(m_pGraphicDev, vecmatWorld);
			m_mapTerrainCubeCom.insert({ texidx, pTerrainTex });
			m_mapComponent[ID_STATIC].insert({ to_wstring(texidx), pTerrainTex });
		}
		else
			iter->second->ReCreateBuffer(vecmatWorld);
	}

	set<_int> Texidx;
	for (auto i : m_vecTotalTex)
		Texidx.insert(i.iTexIdx);

	for (auto texidx : Texidx)
	{
		vector<_matrix> vecmatWorld;

		for (auto i : m_vecTotalTex)
		{
			if (texidx == i.iTexIdx)
				vecmatWorld.push_back(i.matWorld);
		}

		auto iter = m_mapTerrainRcCom.find(texidx);
		if (iter == m_mapTerrainRcCom.end())
		{
			CTerrainRcTex* pTerrainTex = CTerrainRcTex::Create(m_pGraphicDev, vecmatWorld);
			m_mapTerrainRcCom.insert({ texidx, pTerrainTex });
			m_mapComponent[ID_STATIC].insert({ to_wstring(texidx) + L"TEX", pTerrainTex });
		}
		else
			iter->second->ReCreateBuffer(vecmatWorld);
	}

	//충돌처리
	for (auto& coll : m_vecCollision)
	{
		_vec3 vCenter;
		vCenter.x = coll.matWorld._41;
		vCenter.y = coll.matWorld._42;
		vCenter.z = coll.matWorld._43;
		Engine::Add_StaticCollision(vCenter, 0.5f);
	}

	// BlockTex(블럭마다 소리를 다르게 하기 위해)
	for (auto& index : m_vecLand)
	{
		_vec3 vCenter{ 0.f, 0.f, 0.f };
		D3DXVec3TransformCoord(&vCenter, &vCenter, &index.matWorld);
		m_arrBlock[(_int)vCenter.x][(_int)vCenter.z] = index.iTexIdx;
	}

}

void CTerrainCubeMap::SaveMap(const wstring & wstrPath)
{
	//Set data in m_fHeight
	Set_CubeCoordinate();

	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed Save Map");
		return;
	}

	DWORD	dwByte = 0;
	
	size_t m_vecTotalCubeSize = m_vecTotalCube.size();
	size_t m_vecTotalTexSize = m_vecTotalTex.size();

	WriteFile(hFile, &m_vecTotalCubeSize, sizeof(size_t), &dwByte, nullptr);
	WriteFile(hFile, &m_vecTotalTexSize, sizeof(size_t), &dwByte, nullptr);

	for (auto iter : m_vecTotalCube)
		WriteFile(hFile, &iter, sizeof(MapCubeInfo), &dwByte, nullptr);

	for (auto iter : m_vecTotalTex)
		WriteFile(hFile, &iter, sizeof(MapCubeInfo), &dwByte, nullptr);

	WriteFile(hFile, &m_fHeight, sizeof(m_fHeight), &dwByte, nullptr);
		
	CloseHandle(hFile);
}


void CTerrainCubeMap::Set_CubeCoordinate(void)
{
	_ulong	dwIndex = 0;

	
	_float fLength = 0;

	for (auto& itr : m_vecLand)
	{
		_vec3 vCenter{ 0.f, 0.f, 0.f };
		D3DXVec3TransformCoord(&vCenter, &vCenter, &itr.matWorld);
		_float fX = vCenter.x - 0.5f;
		_float fZ = vCenter.z - 0.5f;

		fLength = itr.fHeight / 2.f + vCenter.y;


		if (fLength > m_fHeight[(_int)fX][(_int)fZ])
			m_fHeight[(_int)fX][(_int)fZ] = fLength;
	}
}



void CTerrainCubeMap::AddCube(const MapCubeInfo& tInfo)
{
	m_vecTotalCube.push_back(tInfo);

	switch (tInfo.eType)
	{
	case TYPE_LAND:
		m_vecLand.push_back(tInfo);
		break;
	case TYPE_COLLISION:
		m_vecCollision.push_back(tInfo);
		break;
	case TYPE_DECO:
		break;
	case TYPE_TEX:
		break;
	default:
		_CRASH("Invalid type");
	}

	auto itr = m_mapTerrainCubeCom.find(tInfo.iTexIdx);
	if (itr == m_mapTerrainCubeCom.end())
	{
		vector<_matrix> vecTmp;
		vecTmp.push_back(tInfo.matWorld);

		CTerrainCubeTex* pTerrainTex = CTerrainCubeTex::Create(m_pGraphicDev, vecTmp);
		m_mapTerrainCubeCom.insert({tInfo.iTexIdx, pTerrainTex });
		m_mapComponent[ID_STATIC].insert({ to_wstring(tInfo.iTexIdx), pTerrainTex });
	}
	else
	{
		vector<_matrix> vecTmp;
		for (const auto& info : m_vecTotalCube)
		{
			if (info.iTexIdx == tInfo.iTexIdx)
				vecTmp.push_back(info.matWorld);
		}

		itr->second->ReCreateBuffer(vecTmp);
	}
}


void CTerrainCubeMap::DeleteCube(int iToDel)
{
	int iTexId = m_vecTotalCube[iToDel].iTexIdx;
	m_vecTotalCube.erase(m_vecTotalCube.begin() + iToDel);

	auto itr = m_mapTerrainCubeCom.find(iTexId);
	if (itr == m_mapTerrainCubeCom.end())
	{
		_CRASH("Invalid");
	}

	vector<_matrix> vecTmp;
	for (const auto& info : m_vecTotalCube)
	{
		if (info.iTexIdx == iTexId)
			vecTmp.push_back(info.matWorld);
	}

	
	itr->second->ReCreateBuffer(vecTmp);
}

void CTerrainCubeMap::AddTex(const MapCubeInfo & tInfo)
{
	m_vecTotalTex.push_back(tInfo);

	auto itr = m_mapTerrainRcCom.find(tInfo.iTexIdx);
	if (itr == m_mapTerrainRcCom.end())
	{
		vector<_matrix> vecTmp;
		vecTmp.push_back(tInfo.matWorld);
		
		CTerrainRcTex* pTerrainTex = CTerrainRcTex::Create(m_pGraphicDev, vecTmp);
		m_mapTerrainRcCom.insert({ tInfo.iTexIdx, pTerrainTex });
		m_mapComponent[ID_STATIC].insert({ to_wstring(tInfo.iTexIdx) + L"TEX", pTerrainTex });
	}
	else
	{
		vector<_matrix> vecTmp;
		for (const auto& info : m_vecTotalTex)
		{
			if (info.iTexIdx == tInfo.iTexIdx)
				vecTmp.push_back(info.matWorld);
		}

		itr->second->ReCreateBuffer(vecTmp);
	}
}

void CTerrainCubeMap::DeleteTex(_vec3 PickPos)
{
	_int iCount = 0;
	_int iTexId = 0;

	if (m_vecTotalTex.empty())
		return;


	for(size_t i=0; i<m_vecTotalTex.size(); )
	{	
		_vec3 vectemp = { m_vecTotalTex[i].matWorld._41, m_vecTotalTex[i].matWorld._42, m_vecTotalTex[i].matWorld._43 };
		if (vectemp == PickPos)
		{
			iTexId = m_vecTotalTex[i].iTexIdx;
			m_vecTotalTex.erase(m_vecTotalTex.begin() + iCount);
		}
		else
		{
			i++;
			iCount++;
		}
		
	}

	auto itr = m_mapTerrainRcCom.find(iTexId);
	if (itr == m_mapTerrainRcCom.end())
		return;

	vector<_matrix> vecTmp;
	for (const auto& info : m_vecTotalTex)
	{
		if (info.iTexIdx == iTexId)
			vecTmp.push_back(info.matWorld);
	}


	itr->second->ReCreateBuffer(vecTmp);
}


CTerrainCubeMap * CTerrainCubeMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrPath)
{
	CTerrainCubeMap*	pInstance = new CTerrainCubeMap(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(wstrPath)))
	{
		Safe_Release(pInstance);
		return nullptr;

	}

	return pInstance;
}

void CTerrainCubeMap::Free(void)
{
	CGameObject::Free();
	m_vecTotalCube.clear();
	m_vecLand.clear();
	m_vecCollision.clear();
	m_vecTotalTex.clear();
	m_mapTerrainCubeCom.clear();
	m_mapTerrainRcCom.clear();
}
