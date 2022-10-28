#include "stdafx.h"
#include "SkeletalCube.h"
#include "GameUtilMgr.h"
#include <mutex>
#include "StatComponent.h"

string CSkeletalCube::s_strRoot = "root";
map<wstring, CubeAnimFrame> CubeAnimFrame::s_mapFrame;

CSkeletalCube::CSkeletalCube(LPDIRECT3DDEVICE9 pGraphicDev): CGameObject(pGraphicDev)
{
	ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));

	m_Material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_Material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_Material.Ambient = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.f);
	m_Material.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	m_Material.Power = 0.f;
}

CSkeletalCube::~CSkeletalCube()
{
}

HRESULT CSkeletalCube::Ready_Object()
{
	CComponent* pComponent = nullptr;

	// root
	m_pRootPart = new SkeletalPart;
	// 루트 부분 수정하지 말기
	m_pShaderCom = Add_Component<CShader>(L"Proto_CubeShaderCom", L"Proto_CubeShaderCom", ID_DYNAMIC);
	m_pRootPart->pTrans = Add_Component<CTransform>(L"Proto_TransformCom", L"Proto_TransformCom", ID_DYNAMIC);

	m_pRootPart->strName = s_strRoot;
	m_mapParts.insert({s_strRoot, m_pRootPart});
	m_pRootPart->strTransProto = L"Proto_TransformCom";
	m_pRootPart->strTransCom = L"Proto_TransformCom";
	m_fTime = 0.f;
	return S_OK;
}

_int CSkeletalCube::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	AnimFrameConsume(fTimeDelta);

	Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CSkeletalCube::LateUpdate_Object()
{
	CGameObject::LateUpdate_Object();
}

void CSkeletalCube::Render_Object()
{
	// // m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->Get_WorldMatrixPointer());
 //   m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
 //   m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
 //   m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 //   m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
 //   m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xcc);
 //   m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	_matrix ViewMatrix, ProjMatrix;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_matrix));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_matrix));

	m_pShaderCom->Set_Bool("g_isHit", false);
	m_pShaderCom->Set_Bool("g_isDead", false);

	for (auto& com : m_mapComponent[ID_DYNAMIC])
	{
		if (CStatComponent* pStat = dynamic_cast<CStatComponent*>(com.second))
		{
			m_pShaderCom->Set_Bool("g_isHit", pStat->IsDamaged());
			m_pShaderCom->Set_Bool("g_isDead", pStat->IsDead());
			m_bRenderMachine = !pStat->IsDamaged() && !pStat->IsDead();
			if (pStat->IsDead())
			{
				m_fTime += CGameUtilMgr::s_fTimeDelta * 0.33f;
				m_pShaderCom->Set_RawValue("g_Time", &m_fTime, sizeof(_float));
			}
			break;
		}
	}


	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, m_bLight);
	
	m_pGraphicDev->SetMaterial(&m_Material);
	m_pRootPart->matParents = m_pRootPart->pTrans->m_matWorld;
	for (const auto& child : m_pRootPart->vecChild)
	{
		RenderObjectRecur(child);
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

   // m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
   // m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CSkeletalCube::RenderObjectRecur(SkeletalPart* pPart)
{
	_matrix matPartWorld = pPart->GetWorldMat();

	if (dynamic_cast<CVoxelTex*>(pPart->pBuf) || m_bRenderMachine)
	{

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matPartWorld);
		if (pPart->pTex)
			pPart->pTex->Set_Texture(pPart->iTexIdx);
		if (pPart->pBuf)
			pPart->pBuf->Render_Buffer();
	}
	else
	{
		m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&matPartWorld, &matPartWorld), sizeof(_matrix));
		if (pPart->pTex)
		{
			pPart->pTex->Set_Texture(m_pShaderCom, "g_DefaultTexture", pPart->iTexIdx);
		}
		if (pPart->pBuf)
		{
			m_pShaderCom->Begin_Shader(0);
			pPart->pBuf->Render_Buffer();
			m_pShaderCom->End_Shader();
		}
	}

	for (const auto& child : pPart->vecChild)
		RenderObjectRecur(child);
}

void CSkeletalCube::Free()
{
	CGameObject::Free();
	for_each(m_mapParts.begin(), m_mapParts.end(), [](pair<string, SkeletalPart*> Pair)
	{
		Pair.second->vecChild.clear();
		delete Pair.second;
	});
}

_bool CSkeletalCube::AddSkeletalPart(const string& strPart, const string& strParent, const wstring& strBuf,
                                     const wstring& strTex,
                                     const _uint iTexNum)
{
	const auto itr_dup_check = m_mapParts.find(strPart);
	if (itr_dup_check != m_mapParts.end())
	{
		MSG_BOX("strPart is already exist");
		return false;
	}

	auto itr_parent = m_mapParts.find(strParent);
	if (itr_parent == m_mapParts.end())
	{
		MSG_BOX("Fail to Find strParent");
		return false;
	}

	SkeletalPart* pPart = new SkeletalPart;
	wstring wstrPart;
	wstrPart.assign(strPart.begin(), strPart.end());

	pPart->strBufCom = strBuf + L"_" + wstrPart;
	pPart->strBufProto = strBuf;
	pPart->pBuf = Add_Component<CVIBuffer>(pPart->strBufProto, pPart->strBufCom, ID_STATIC);

	pPart->strTexCom = strTex + L"_" + wstrPart;
	pPart->strTexProto = strTex;
	pPart->pTex = Add_Component<CTexture>(pPart->strTexProto, pPart->strTexCom, ID_STATIC);

	pPart->strTransCom = L"Proto_TransformCom_" + wstrPart;
	pPart->strTransProto = L"Proto_TransformCom";
	pPart->pTrans = Add_Component<CTransform>(pPart->strTransProto, pPart->strTransCom, ID_STATIC);

	pPart->iTexIdx = iTexNum;
	pPart->pParent = (*itr_parent).second;
	pPart->strName = strPart;
	m_mapParts.insert({strPart, pPart});
	pPart->pParent->vecChild.push_back(pPart);

	return true;
}

_bool CSkeletalCube::DeleteSkeletalPart(const string& strPart)
{
	auto itr_part = m_mapParts.find(strPart);
	if (itr_part == m_mapParts.end())
	{
		MSG_BOX("Fail to Find strPart");
		return false;
	}
	if (strPart == s_strRoot)
	{
		MSG_BOX("Do not delete root");
		return false;
	}

	SkeletalPart* pToDelete = (*itr_part).second;
	SkeletalPart* pParent = pToDelete->pParent;
	pParent->vecChild.erase(
		remove_if(pParent->vecChild.begin(), pParent->vecChild.end(),
		          [strPart](const SkeletalPart* pChild)
		          {
			          if (pChild->strName == strPart)
				          return true;
			          return false;
		          }),
		pParent->vecChild.end());

	DeleteRecursive(strPart);

	return true;
}

CSkeletalCube* CSkeletalCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring wstrPath)
{
	CSkeletalCube* pInstance = new CSkeletalCube(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	if (!wstrPath.empty())
		pInstance->LoadSkeletal(wstrPath);

	return pInstance;
}

void CSkeletalCube::AnimFrameConsume(_float fTimeDelta)
{
	if (m_bStopAnim)
		return;

	fTimeDelta *= m_fAnimSpeed;

	if (m_pCurAnim == nullptr) return;

	
	for (const auto& animEvent : m_pCurAnim->vecEvent)
	{
		if (m_fAccTime <= animEvent.first && m_fAccTime + fTimeDelta >= animEvent.first)
		{
			AnimationEvent(animEvent.second);
			break;
		}
	}

	m_fAccTime += fTimeDelta;
	if (m_pCurAnim->fTotalTime < m_fAccTime)
	{
		if (m_bReserveStop)
		{
			m_bStopAnim = true;
			m_bReserveStop = false;
			AnimationEvent("AnimStopped");
			return;
		}

		m_fAccTime = 0.f;
		if (m_pCurAnim->bLoop == false) // loop 가 아니면 다 실행하고 loop로 변경
		{
			m_pCurAnim = m_pIdleAnim;
			m_pCurAnim->bLoop = true;
			return;
		}
	}

	for (const auto& Part : m_mapParts)
	{
		auto itr_frame = m_pCurAnim->mapFrame.find(Part.second->strName);
		if (itr_frame == m_pCurAnim->mapFrame.end())
			continue;

		vector<TransFrame>& vecTransFrame = itr_frame->second;

		const TransFrame* pPrevFrame = nullptr;
		const TransFrame* pNextFrame = nullptr;
		
		for (auto& frame : vecTransFrame)
		{
			const _float fTime = frame.fTime;
			if (fTime < m_fAccTime)
			{
				pPrevFrame = &frame;
			}
			else if (fTime > m_fAccTime)
			{
				pNextFrame = &frame;
				break;
			}
		}
		if (pPrevFrame == nullptr)
			continue;
		if (pNextFrame == nullptr)
		{
			CGameUtilMgr::MatWorldCompose(
				OUT Part.second->pTrans->m_matWorld,
				pPrevFrame->vScale,
				pPrevFrame->qRot,
				pPrevFrame->vPos);
			continue;
		}
		
		const _float fS = (m_fAccTime - pPrevFrame->fTime) / (pNextFrame->fTime - pPrevFrame->fTime);
		TransFrameLerp(
			OUT Part.second->pTrans->m_matWorld, 
			*pPrevFrame, 
			*pNextFrame, 
			fS);
	}
}

void CSkeletalCube::PlayAnimationOnce(CubeAnimFrame* frame, bool bReserveStop)
{
	m_fAccTime = 0.f;
	m_pCurAnim = frame;
	m_pCurAnim->bLoop = false;
	m_bReserveStop = bReserveStop;
}

void CSkeletalCube::PlayAnimationLoop(CubeAnimFrame* frame)
{
	m_fAccTime = 0.f;
	m_pCurAnim = frame;
	m_pCurAnim->bLoop = true;
	m_pIdleAnim = frame;
	m_pIdleAnim->bLoop = true;
}

void CSkeletalCube::StopCurAnimation()
{
	if (m_pCurAnim)
	{
		m_fAccTime = 0.f;
		m_pCurAnim = m_pIdleAnim;
	}
}

void CSkeletalCube::LoadSkeletal(wstring wstrPath)
{
	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		string tmp;
		tmp.assign(wstrPath.begin(), wstrPath.end());
		MSG_BOX("Fail to Load SkeletalCube");
		IM_LOG(tmp.c_str());
		return;
	}
	m_wstrPath = wstrPath;

	for (auto& child : m_pRootPart->vecChild)
	{
		string strChild = child->strName;
		DeleteRecursive(strChild);
	}
	m_pRootPart->vecChild.clear();


	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	size_t iPartCnt = 0;

	ReadFile(hFile, &iPartCnt, sizeof(size_t), &dwByte, nullptr);

	for (size_t i = 0; i < iPartCnt; ++i)
	{
		string strParent;
		string strPart;
		wstring wstrBufProto;
		wstring wstrTexProto;

		_uint iTexNum;
		_matrix matLocal;
		_matrix matWorld;

		// read parent
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char szParent[128]{};
		ReadFile(hFile, szParent, dwStrByte, &dwByte, nullptr);
		strParent = szParent;

		// read part name
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char szPart[128]{};
		ReadFile(hFile, szPart, dwStrByte, &dwByte, nullptr);
		strPart = szPart;

		// read buf proto
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		_tchar wszBufProto[128]{};
		ReadFile(hFile, wszBufProto, dwStrByte, &dwByte, nullptr);
		wstrBufProto = wszBufProto;

		// read tex proto
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		_tchar wszTexProto[128]{};
		ReadFile(hFile, wszTexProto, dwStrByte, &dwByte, nullptr);
		wstrTexProto = wszTexProto;

		// read tex num
		ReadFile(hFile, &iTexNum, sizeof(_uint), &dwByte, nullptr);

		AddSkeletalPart(strPart, strParent, wstrBufProto, wstrTexProto, iTexNum);

		// local, world mat
		ReadFile(hFile, &matLocal, sizeof(_matrix), &dwByte, nullptr);
		ReadFile(hFile, &matWorld, sizeof(_matrix), &dwByte, nullptr);
		
		const auto itr_part = m_mapParts.find(strPart);
		if (itr_part != m_mapParts.end())
		{
			(*itr_part).second->matLocal = matLocal;
			(*itr_part).second->pTrans->m_matWorld = matWorld;
		}
	}

	CloseHandle(hFile);
}

void CSkeletalCube::SaveSkeletal(wstring wstrPath)
{
	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Fail to Save SkeletalCube");
		return;
	}
	DWORD dwByte = 0;

	size_t iPartCnt = m_mapParts.size() - 1; // except root

	WriteFile(hFile, &iPartCnt, sizeof(size_t), &dwByte, nullptr);

	for (const auto& child : m_pRootPart->vecChild)
	{
		SaveRecursive(hFile, child);
	}

	CloseHandle(hFile);
}

void CSkeletalCube::SaveRecursive(HANDLE hFile, SkeletalPart* pPart)
{
	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	// parent name
	dwStrByte = (DWORD)pPart->pParent->strName.size();
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, pPart->pParent->strName.c_str(), dwStrByte, &dwByte, nullptr);

	// part name
	dwStrByte = (DWORD)pPart->strName.size();
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, pPart->strName.c_str(), dwStrByte, &dwByte, nullptr);

	// Buf proto name
	if (pPart->strName == "bow")
	{
		wstring pro = L"Proto_VoxelTex_Bow";
	
	dwStrByte = (DWORD)pro.size() * sizeof(_tchar);
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, pro.c_str(), dwStrByte, &dwByte, nullptr);
	}
	else
	{
	dwStrByte = (DWORD)pPart->strBufProto.size() * sizeof(_tchar);
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, pPart->strBufProto.c_str(), dwStrByte, &dwByte, nullptr);
	}



	// Tex proto name, idx
	dwStrByte = (DWORD)pPart->strTexProto.size() * sizeof(_tchar);
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, pPart->strTexProto.c_str(), dwStrByte, &dwByte, nullptr);
	WriteFile(hFile, &pPart->iTexIdx, sizeof(_uint), &dwByte, nullptr);

	// mat local
	WriteFile(hFile, &pPart->matLocal, sizeof(_matrix), &dwByte, nullptr);

	// trans world
	WriteFile(hFile, &pPart->pTrans->m_matWorld, sizeof(_matrix), &dwByte, nullptr);

	for (const auto& child : pPart->vecChild)
	{
		SaveRecursive(hFile, child);
	}
}

void CSkeletalCube::DeleteRecursive(const string& strPart)
{
	auto itr_part = m_mapParts.find(strPart);
	if (itr_part == m_mapParts.end())
	{
		MSG_BOX("Fail to Find strPart");
		return;
	}
	SkeletalPart* pToDelete = (*itr_part).second;

	for (auto& child : pToDelete->vecChild)
	{
		string strChild = child->strName;
		DeleteRecursive(strChild);
	}
	pToDelete->vecChild.clear();

	pToDelete->pBuf->Release();
	pToDelete->pTex->Release();
	pToDelete->pTrans->Release();

	{
		auto itr = m_mapComponent[ID_STATIC].find(pToDelete->strBufCom);
		if (itr != m_mapComponent[ID_STATIC].end())
			m_mapComponent[ID_STATIC].erase(itr);
	}
	{
		auto itr = m_mapComponent[ID_STATIC].find(pToDelete->strTexCom);
		if (itr != m_mapComponent[ID_STATIC].end())
			m_mapComponent[ID_STATIC].erase(itr);
	}
	{
		auto itr = m_mapComponent[ID_STATIC].find(pToDelete->strTransCom);
		if (itr != m_mapComponent[ID_STATIC].end())
			m_mapComponent[ID_STATIC].erase(itr);
	}

	m_mapParts.erase(strPart);
	delete pToDelete;
}

void CSkeletalCube::TransFrameLerp(_matrix& matOut, const TransFrame& PrevFrame, const TransFrame& NextFrame,
	const _float fS)
{
	const _vec3 vPrevScale = PrevFrame.vScale;
	const D3DXQUATERNION qPrevQuat = PrevFrame.qRot;
	const _vec3 vPrevPos = PrevFrame.vPos;

	const _vec3 vNextScale = NextFrame.vScale;
	const D3DXQUATERNION qNextQuat = NextFrame.qRot;
	const _vec3 vNextPos = NextFrame.vPos;

	_vec3 vLerpScale = vPrevScale;
	D3DXQUATERNION qLerpQuat = qPrevQuat;
	_vec3 vLerpPos = vPrevPos;

	D3DXVec3Lerp(&vLerpScale, &vPrevScale, &vNextScale, fS);
	D3DXQuaternionSlerp(&qLerpQuat, &qPrevQuat, &qNextQuat, fS);
	D3DXVec3Lerp(&vLerpPos, &vPrevPos, &vNextPos, fS);

	CGameUtilMgr::MatWorldCompose(matOut, vLerpScale, qLerpQuat, vLerpPos);
}

/*----------------------
 *     CubeAnimFrame
 -----------------------*/
CubeAnimFrame CubeAnimFrame::Load(const wstring& wstrPath)
{
	CubeAnimFrame tmp;

	if (s_mapFrame.find(wstrPath) != s_mapFrame.end())
		return s_mapFrame.find(wstrPath)->second;


	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Fail to Load CubeAnimFrame");
		return tmp;
	}

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	size_t mapSize;

	string strPartName;
	size_t vecSize;
	TransFrame tFrame;

	ReadFile(hFile, &tmp.bLoop, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &tmp.fTotalTime, sizeof(_float), &dwByte, nullptr);

	ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

	for (size_t i = 0; i < mapSize; ++i)
	{
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char szPartName[128]{};
		ReadFile(hFile, szPartName, dwStrByte, &dwByte, nullptr);
		strPartName = szPartName;

		vector<TransFrame> vecFrameTmp;
		ReadFile(hFile, &vecSize, sizeof(size_t), &dwByte, nullptr);

		for (size_t j = 0; j < vecSize; ++j)
		{
			ReadFile(hFile, &tFrame.fTime, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &tFrame.vScale, sizeof(_vec3), &dwByte, nullptr);
			ReadFile(hFile, &tFrame.qRot, sizeof(D3DXQUATERNION), &dwByte, nullptr);
			ReadFile(hFile, &tFrame.vPos, sizeof(_vec3), &dwByte, nullptr);
			vecFrameTmp.push_back(tFrame);
		}

		tmp.mapFrame.insert({strPartName, vecFrameTmp});
	}

	size_t eventSize = 0;
	ReadFile(hFile, &eventSize, sizeof(size_t), &dwByte, nullptr);

	_float fEventTime;
	for (size_t i = 0; i < eventSize; ++i)
	{
		ReadFile(hFile, &fEventTime, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char szEventName[128]{};
		ReadFile(hFile, szEventName, dwStrByte, &dwByte, nullptr);
		string strEventName = szEventName;

		tmp.vecEvent.push_back({fEventTime, strEventName});
	}

	CloseHandle(hFile);

	s_mapFrame.insert({wstrPath, tmp});

	return tmp;
}

void CubeAnimFrame::Save(const wstring& wstrPath)
{
	HANDLE hFile = CreateFile(wstrPath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Fail to Save CubeAnimFrame");
		return;
	}

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	WriteFile(hFile, &bLoop, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &fTotalTime, sizeof(_float), &dwByte, nullptr);

	size_t mapSize = mapFrame.size();
	WriteFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

	for (const auto& frame : mapFrame)
	{
		// target part name
		dwStrByte = (DWORD)frame.first.size();
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, frame.first.c_str(), dwStrByte, &dwByte, nullptr);

		size_t vecSize = frame.second.size();
		WriteFile(hFile, &vecSize, sizeof(size_t), &dwByte, nullptr);

		for (const auto& trans : frame.second)
		{
			WriteFile(hFile, &trans.fTime, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &trans.vScale, sizeof(_vec3), &dwByte, nullptr);
			WriteFile(hFile, &trans.qRot, sizeof(D3DXQUATERNION), &dwByte, nullptr);
			WriteFile(hFile, &trans.vPos, sizeof(_vec3), &dwByte, nullptr);
		}
	}

	size_t eventSize = vecEvent.size();
	WriteFile(hFile, &eventSize, sizeof(size_t), &dwByte, nullptr);
	for (size_t i = 0; i < eventSize; ++i)
	{
		
		WriteFile(hFile, &vecEvent[i].first, sizeof(_float), &dwByte, nullptr);
		dwStrByte = (DWORD)vecEvent[i].second.size();
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, vecEvent[i].second.c_str(), dwStrByte, &dwByte, nullptr);
		
	}
	
	CloseHandle(hFile);
}


void CubeAnimFrame::SortFrame(const string& strPart)
{
	auto& vecFrame = mapFrame.find(strPart)->second;
	sort(vecFrame.begin(), vecFrame.end(), [](const TransFrame& a, const TransFrame& b)
	{
		return a.fTime < b.fTime;
	});

	sort(vecEvent.begin(), vecEvent.end());
}
