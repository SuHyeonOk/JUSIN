#pragma once
#include "Engine_Include.h"
#include "Base.h"
//
// BEGIN(Engine)
//
// #pragma region Particle Struct
// typedef struct tagParticle
// {
// 	_vec3		_Pos;
// 	D3DCOLOR	_Color;
// 	static const DWORD ParticleFVF;
// }VTXPARTICLE;
// #pragma endregion
//
// #pragma region Color Variable
// const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
// const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
// const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
// const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
// const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
// const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
// const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
// const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
// #pragma endregion
//
// #pragma region BoundingBox
// // class ENGINE_DLL BoundingBox
// // {
// // public:
// // 	BoundingBox()
// // 	{
// // 		_min.x = FLT_MAX;
// // 		_min.y = FLT_MAX;
// // 		_min.z = FLT_MAX;
// //
// // 		_max.x = -FLT_MAX;
// // 		_max.y = -FLT_MAX;
// // 		_max.z = -FLT_MAX;
// // 	}
// //
// // 	bool IsPointInside(_vec3& _point);
// //
// // 	_vec3 _min;
// // 	_vec3 _max;
// // };
// #pragma endregion
//
// #pragma region BoundingSphere
// // class ENGINE_DLL BoundingSphere
// // {
// // public:
// // 	BoundingSphere();
// //
// // 	_vec3	_center;
// // 	_float  _radius;
// // };
// #pragma endregion
//
// class CParticleMgr :public CBase
// {
// 	DECLARE_SINGLETON(CParticleMgr)
// private:
// 	explicit CParticleMgr();
// 	virtual ~CParticleMgr();
//
// public:
// 	void		Add_Particle(const _vec3& _pos, const _float& _size,
// 							const D3DXCOLOR& _color, const _uint& _numParticle, 
// 							const _float& _lifetime);
//
// 	void		Render_Particle(LPDIRECT3DDEVICE9& pGraphicDev);
// 	void		PreRender_Particle(void);
// 	void		PostRender_Particle(void);
// 	void		RemoveDeadParticles(void);
//
// public:
//
// private:
// 	_vec3							m_vOrigin;
// 	//BoundingBox						m_bBoundingBox;
//
// 	_float							m_fEmitRate;
// 	_float							m_fSize;
// 	IDirect3DTexture9*				m_pTexture;
// 	IDirect3DVertexBuffer9*			m_pVtxBf;
// 	//std::list<Attribute>			m_ParticlesList;
// 	_int							m_iMaxParticles;
//
// 	DWORD							m_dwVtxBf_Size;
// 	DWORD							m_dwVtxBf_Offset;
// 	DWORD							m_dwVtxBf_BatchSize;
//
// private:
// 	virtual void Free(void);
// };
//
// END