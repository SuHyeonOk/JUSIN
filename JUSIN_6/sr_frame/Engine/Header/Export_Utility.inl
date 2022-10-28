
void SwitchSceneLoading(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay)
{
	CManagement::GetInstance()->SwitchSceneLoading(pLoading, pSceneCreate, delay);
}
void SwitchSceneLoadingDeletePrev(CScene* pLoading, std::function<CScene*()>& pSceneCreate, long long delay)
{
	CManagement::GetInstance()->SwitchSceneLoadingDeletePrev(pLoading, pSceneCreate, delay);
}

HRESULT	Create_Management(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	*ppManagement = pManagement;

	return S_OK;
}

HRESULT		Set_Scene(CScene* pScene)
{
	return CManagement::GetInstance()->Set_Scene(pScene);
}

_int		Update_Scene(const _float& fTimeDelta)
{
	return CManagement::GetInstance()->Update_Scene(fTimeDelta);
}
void		LateUpdate_Scene(void)
{
	CManagement::GetInstance()->LateUpdate_Scene();
}
void		Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CManagement::GetInstance()->Render_Scene(pGraphicDev);
}

inline void Get_AllGameObject(LAYERID eLayerID, const wstring& pObjTag, list<CGameObject*>& outList)
{
	CManagement::GetInstance()->Get_AllGameObject(eLayerID, pObjTag, outList);
}

inline CLayer* Get_Layer(LAYERID eLayerID)
{
	return CManagement::GetInstance()->Get_Layer(eLayerID);
}

void AddGameObject(LAYERID eLayerID, const wstring& pObjTag, CGameObject* pObject)
{
	CManagement::GetInstance()->AddGameObject(eLayerID, pObjTag, pObject);
}

inline HRESULT			Ready_Proto(const wstring& pProtoTag, CComponent* pComponent)
{
	return CProtoMgr::GetInstance()->Ready_Proto(pProtoTag, pComponent);
}

inline CComponent*		Clone_Proto(const wstring& pProtoTag)
{
	return CProtoMgr::GetInstance()->Clone_Proto(pProtoTag);
}

inline CComponent* Find_Proto(const wstring& pProtoTag)
{
	return CProtoMgr::GetInstance()->Find_Proto(pProtoTag);
}

void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Add_RenderGroup(eID, pGameObject);
}
void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
}
void		Clear_RenderGroup(void)
{
	CRenderer::GetInstance()->Clear_RenderGroup();
}


void Add_CollisionCom(CCollisionCom* pCollision)
{
	CCollider::GetInstance()->Add_CollisionCom(pCollision);
}
void Add_StaticCollision(const _vec3& vCenter, _float fRadius)
{
	CCollider::GetInstance()->Add_StaticCollision(vCenter, fRadius);
}
void GetOverlappedObject(OUT set<CGameObject*>& objList, const _vec3& vPos, _float fRadius)
{
	CCollider::GetInstance()->GetOverlappedObject(objList, vPos, fRadius);
}
void Clear_ColliderAll()
{
	CCollider::GetInstance()->Clear_ColliderAll();
}

inline HRESULT Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}


inline void			Release_Utility(void)
{
	CLightMgr::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
	CCollider::GetInstance()->DestroyInstance();
}
