#pragma once
#include "ParticleSystem.h"

#pragma region Particle System
class CAttack_P :public CParticleSystem
{
public:
	explicit CAttack_P(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {};
	virtual ~CAttack_P() override;
public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;

public:
	static CAttack_P* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;
};


// class CTrajectory : public CParticleSystem
// {
// public:
// 	explicit CTrajectory(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
// 	~CTrajectory() override;
//
// public:
// 	_int Update_Object(const _float& fTimeDelta) override;
// 	void Render_Object() override;
// 	void Reset_Particle(Attribute* _Attribute) override;
// 	void PreRender_Particle() override;
// 	void PostRender_Particle() override;
//
// 	HRESULT		SetUp_Material(void);
// public:
// 	static CTrajectory* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName, _vec3* _origin, _int _numParticles);
//
// 	void Free() override;
// };
//


class CSpeedBoots : public CParticleSystem
{
public:
	explicit CSpeedBoots(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CSpeedBoots() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;

public:
	static CSpeedBoots* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;
};
enum BaseType
{
	SPEEDBOOTS = 0,
	ITEMDROP	
};
class C3DBaseTexture : public CParticleSystem
{
public:
	explicit C3DBaseTexture(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~C3DBaseTexture() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;

public:
	static C3DBaseTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;
};

class CSpeedBoots_Particle : public CParticleSystem
{
public:
	explicit CSpeedBoots_Particle(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CSpeedBoots_Particle() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;

public:
	static CSpeedBoots_Particle* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;

private:
	_float m_fSpeed;
	_float m_fRotAngle;
	_float m_fRotSpeed;
	_float m_fDiagonal;
	_float m_fScale;
};

class CFireWork_Fuze : public CParticleSystem
{
public:
	explicit CFireWork_Fuze(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CFireWork_Fuze() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;

	const _vec3& Get_FuzePos(void) { return vDeadPos; }

public:
	static CFireWork_Fuze* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;
public:
	_vec3 vDeadPos;

};

class CFireWork : public CParticleSystem
{
public:
	explicit CFireWork(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CFireWork() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;
	void Reset_tmp(void) { tmp = 1; }
public:
	static CFireWork* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;

private:
	_uint tmp = 1;
};

class CFireWork_Kouku : public CParticleSystem
{
public:
	explicit CFireWork_Kouku(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CFireWork_Kouku() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;
	void Reset_tmp(void) { tmp = 1; }
public:
	static CFireWork_Kouku* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;

private:
	_uint tmp = 1;
	_float m_fAngle = 0.f;
};

class CMoonParticle : public CParticleSystem
{
public:
	explicit CMoonParticle(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CMoonParticle() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;
	// void Reset_tmp(void) { tmp = 1; }
public:
	static CMoonParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;

private:
};

class CFascinated_Effect : public CParticleSystem
{
public:
	explicit CFascinated_Effect(LPDIRECT3DDEVICE9 pGraphicDev) : CParticleSystem(pGraphicDev) {}
	virtual ~CFascinated_Effect() override;

public:
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void Reset_Particle(Attribute* _Attribute) override;
	void PreRender_Particle() override;
	void PostRender_Particle() override;
public:
	static CFascinated_Effect* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPCWSTR _TexFileName);

	void Free() override;

private:
};

class CFascinate : public CGameObject
{
public:
   explicit CFascinate(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
   ~CFascinate() override;

public:
   virtual HRESULT Ready_Object(_float _size);
   _int Update_Object(const _float& fTimeDelta) override;
   void Render_Object() override;
   void LateUpdate_Object() override;
   void PreRender_Particle();
   void PostRender_Particle();
   void SetPos(const _vec3& vPos);
   void SetDead() { m_fCurTime = m_fTime; };

public:
   static CFascinate* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
   CRcShader*         m_pBufferCom = nullptr;
   CTransform*         m_pTransCom = nullptr;
   CTexture*         m_pTexture = nullptr;
   void Free() override;

private:
   _float m_fTime;
   _float m_fCurTime;
   _float m_fSpeed;
};
#pragma endregion



#pragma region UVAnimation(RcShader)
class CShock_Powder : public CGameObject
{
public:
	explicit CShock_Powder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShock_Powder() override;

public:
	virtual HRESULT Ready_Object() override;
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CShock_Powder* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CRcShader*			m_pBufferCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime = 1.f;
	_float m_fCurTime = 0.f;
	_bool m_bDoOnce = true;
};

class CChainLightning : public CGameObject
{
public:
	explicit CChainLightning(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CChainLightning() override;

public:
	virtual HRESULT Ready_Object() override;
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetSpark();
	void SetSparkPos(const _vec3& vBot, const _vec3& vTop);

public:
	static CChainLightning* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CRcShader*			m_pBufferCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime = 1.f;
	_float m_fCurTime = 0.f;
	_float m_fOffset;
	_bool m_bSpark = false;
};


enum CIRCLETYPE
{
	SHOCK = 0,
	FIREWORK,
	CREEPER,
	GOLEM
};
class CUVCircle : public CGameObject
{
	
public:
	explicit CUVCircle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CUVCircle() override;

public:
	virtual HRESULT Ready_Object(_float _size, CIRCLETYPE _type);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CUVCircle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, CIRCLETYPE _type);
	CRcShader*			m_pBufferCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;


private:
	_float m_fTime;
	_float m_fCurTime;
	_vec3	m_vVelocity;

	CFireWork_Fuze* m_pFuze;
};

enum CLOUDTYPE
{
	WALK=0,
	ROLL,
	SHOCKPOWDER,
	CREEPEREX,
	DECAL,
	MONSTERCLOUD,
	GOLEMWINDMILL
};


class CCloud : public CGameObject
{
public:
	explicit CCloud(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CCloud() override;

public:
	virtual HRESULT Ready_Object(_float _size, CLOUDTYPE _type) ;
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CCloud* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, CLOUDTYPE _type);
	CRcShader*			m_pBufferCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;
	void SetMatrix(_matrix* matWorld) { m_pTransCom->Set_WorldMatrix(matWorld); }

private:
	_float m_fTime;
	_float m_fCurTime;
	_vec3	m_vVelocity;
	_float m_fSpeed;
};

enum CRACKTYPE
{
	GOLEM_SPIT_CRACK = 0,
	EXE_DECAL,
	LAZER,
	LOADINGBOX
};

class CCrack : public CGameObject
{
public:
	explicit CCrack(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CCrack() override;

public:
	virtual HRESULT Ready_Object( _float _size, CRACKTYPE _type);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CCrack* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, CRACKTYPE _type);
	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_uint m_iCrackType;

	//_vec3	m_vVelocity;
	//_float m_fSpeed;
};

class CGolemSpit : public CGameObject
{
public:
	explicit CGolemSpit(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CGolemSpit() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CGolemSpit* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_vec3	m_vVelocity;
	_float m_fSpeed;
};

class CStun : public CGameObject
{
public:
	explicit CStun(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CStun() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetPos(const _vec3& vPos);
	void SetDead() {m_fCurTime = m_fTime; };

public:
	static CStun* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
};



class CAttack_Range_Circle : public CGameObject
{
public:
	explicit CAttack_Range_Circle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CAttack_Range_Circle() override;

public:
	virtual HRESULT Ready_Object(const ATKRNGOPTION& circleOption);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetDead() { m_fCurTime = m_fTime; };
	void SetLerp(ATKRNGOPTION* _circleoption);

public:
	static CAttack_Range_Circle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const ATKRNGOPTION& circleOption);

	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
	_float tmp;
	ATKRNGOPTION m_ATKRNGOption;

	// _float m_fAcc;
	// _float m_fMaxAcc;
	//
	// _vec3 m_fMinSize;
	// _vec3 m_fMaxSize;
	//
	//
	//
	// _uint m_iNextFrame;
	// _vec3 m_LerpScale;
	//
	// ATTACKCIRCLETYPE m_eType;
	
};






enum HealCircleType
{
	HEAL,
	BLUE_CIRCLE,
	RED_CIRCLE
};
class CHealCircle : public CGameObject
{
public:
	explicit CHealCircle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CHealCircle() override;

public:
	virtual HRESULT Ready_Object(_float _size,_float _rad, HealCircleType _type);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetFollow(CTransform* pFollow) {m_pFollow = pFollow; m_pFollow->AddRef();}

public:
	static CHealCircle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, _float _rad, HealCircleType _type);
	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
	_float m_fRad;

	CTransform* m_pFollow = nullptr;

	HealCircleType m_eType;
};

class CHeartParticle : public CGameObject
{
public:
	explicit CHeartParticle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CHeartParticle() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetFollow(CTransform* pFollow) {m_pFollow = pFollow; m_pFollow->AddRef();}

public:
	static CHeartParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CRcShader*			m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void				Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
	_vec3	m_vOrigin;

	_float tmp;
	_float desk;

	CTransform* m_pFollow = nullptr;
};


enum LAVATYPE
{
	FALLINLAVA=0,
	FUZEPARTICLE
};

class CLava_Particle : public CGameObject
{
public:
	explicit CLava_Particle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CLava_Particle() override;

public:
	virtual HRESULT Ready_Object(_float _size, LAVATYPE _type);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetFlame();
	void SetFlamePos(const _vec3& vBot, const _vec3& vTop);

public:
	static CLava_Particle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size, LAVATYPE _type);
	CRcShader*				m_pBufferCom = nullptr;
	Engine::CTransform*		m_pTransCom = nullptr;
	CTexture*				m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime = 1.f;
	_float m_fCurTime = 0.f;
	_float m_fOffset;
	_bool m_bFlame = false;
};

class CLazer : public CGameObject
{
public:
	explicit CLazer(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CLazer() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CLazer* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CTerrainShader*		m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;
	void KillLaser() { m_bDead = true; }

private:
	_bool m_bDead = false;
	_float m_fSpeed;
	_vec3  m_vVelocity; 
};

class CGradation_Beam : public CGameObject
{
public:
	explicit CGradation_Beam(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CGradation_Beam() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();
	void SetTransform(_vec3 vPos) { m_pTransCom->m_vInfo[INFO_POS] = vPos; }
	void IsDead() { m_bDead = true; }

public:
	static CGradation_Beam* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CTerrainShader*		m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
	_vec3  m_vVelocity;
	_bool  m_bDead = false;
};

class CLazer_Circle : public CGameObject
{
public:
	explicit CLazer_Circle(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev) {}
	~CLazer_Circle() override;

public:
	virtual HRESULT Ready_Object(_float _size);
	_int Update_Object(const _float& fTimeDelta) override;
	void Render_Object() override;
	void LateUpdate_Object() override;
	void PreRender_Particle();
	void PostRender_Particle();

public:
	static CLazer_Circle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _size);
	CRcShader*		m_pBufferCom = nullptr;
	CTransform*			m_pTransCom = nullptr;
	CTexture*			m_pTexture = nullptr;
	void Free() override;

private:
	_float m_fTime;
	_float m_fCurTime;
	_float m_fSpeed;
	_vec3  m_vVelocity;
};


#pragma endregion

