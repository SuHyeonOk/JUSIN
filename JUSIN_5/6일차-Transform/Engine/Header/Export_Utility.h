#ifndef Export_Utility_h__
#define Export_Utility_h__

#include "Engine_Include.h"
#include "TriCol.h"
#include "RcCol.h"
#include "Transform.h"

#include "ProtoMgr.h"
#include "Management.h"
#include "Renderer.h"

BEGIN(Engine)

// Management


inline HRESULT	Create_Management(LPDIRECT3DDEVICE9& pGraphicDev, CManagement** ppManagement);
inline HRESULT		Set_Scene(CScene* pScene);
inline _int		Update_Scene(const _float& fTimeDelta);
inline void		LateUpdate_Scene(void);
inline void		Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);


// ProtoMgr
inline HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent);
inline CComponent*		Clone_Proto(const _tchar* pProtoTag);


// Renderer
inline void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
inline void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void		Clear_RenderGroup(void);


inline void			Release_Utility(void);

#include "Export_Utility.inl"

END
#endif 
