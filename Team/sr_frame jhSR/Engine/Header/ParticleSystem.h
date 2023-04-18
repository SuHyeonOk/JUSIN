#pragma once

#include "Engine_Include.h"
#include "GameObject.h"

BEGIN(Engine)

typedef struct tagParticle
{
	_vec3		_Pos;
	D3DCOLOR	_Color;
	static const DWORD ParticleFVF;
}VTXPARTICLE;

#pragma region Color Variable
const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
#pragma endregion 

#pragma region BoundingBox
class ENGINE_DLL BoundingBox
{
public:
	BoundingBox();

	bool IsPointInside(_vec3& _point);

	_vec3 _min;
	_vec3 _max;
};
#pragma endregion 

#pragma region BoundingSphere
class ENGINE_DLL BoundingSphere
{
public:
	BoundingSphere();

	_vec3	_center;
	_float  _radius;
};
#pragma endregion 

class ENGINE_DLL CParticleSystem : public CGameObject
{
public:
	CParticleSystem();
	explicit CParticleSystem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParticleSystem();

	HRESULT Ready_Object(LPCWSTR _TexFileName);

	_int			Update_Object(const _float& fTimeDelta) override;
	void			Render_Object() override;
	void			Free() override;

	virtual void	reset(void);												

	virtual void	Reset_Particle(Attribute* _Attribute) PURE;		
	virtual void	Add_Particle(const _vec3& _pos, const _float& _size, const D3DXCOLOR& _color, const _uint& _numParticle, const _float& _lifetime,const _int& _type = 0);

	virtual void	PreRender_Particle(void);	
	virtual void	PostRender_Particle(void);	

	bool			IsEmpty(void);		
	bool			IsDead(void);		 
										 

protected:
	virtual void	RemoveDeadParticles(void);		

protected:
	_vec3							m_vOrigin;		
	BoundingBox						m_bBoundingBox;	
													
	_float							m_fEmitRate;   
	_float							m_fSize;       
	IDirect3DTexture9*				m_pTexture; 
	IDirect3DVertexBuffer9*			m_pVtxBf;	
	std::list<Attribute>			m_ParticlesList;		
	_int							m_iMaxParticles; 

	DWORD							m_dwVtxBf_Size;      
	DWORD							m_dwVtxBf_Offset;    
	DWORD							m_dwVtxBf_BatchSize; 

	// Constants
	const float INFINITY_D3D = FLT_MAX;
	const float EPSILON = 0.001f;
	// ~Constants

	// Randomness
	float GetRandomFloat(float lowBound, float highBound);

	void GetRandomVector(
		_vec3* out,
		_vec3* min,
		_vec3* max);
	// ~Randomness

	// Conversion
	DWORD FtoDw(float f);
	// ~Conversion

	// Materials
	D3DMATERIAL9 InitMtrl(D3DXCOLOR _ambient, D3DXCOLOR _diffuse, D3DXCOLOR _specular, D3DXCOLOR _emissive, float _power);

	const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
	// ~Materials
};

END