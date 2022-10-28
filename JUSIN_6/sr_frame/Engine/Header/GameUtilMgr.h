#pragma once
#include "Engine_Include.h"
BEGIN(Engine)



class ENGINE_DLL CGameUtilMgr
{
public:
	static void WorldMatrixLerp(OUT _matrix& matOut, const _matrix& matPrev, const _matrix& matNext, const _float fS);
	static void QuatToPitchYawRoll(const D3DXQUATERNION& q, _float& pitch, _float& yaw, _float& roll);
	static bool Vec3Cmp(const _vec3& v1, const _vec3& v2, _float fEpsilon = 0.000001f);
	static bool QuatCmp(const D3DXQUATERNION& q1, const D3DXQUATERNION& q2, _float fEpsilon = 0.000001f);
	static bool MatCmp(const _matrix& m1, const _matrix& m2, _float fEpsilon = 0.000001f);
	static bool FloatCmp(const _float& f1, const _float f2, _float fEpsilon = 0.0000001f);
	static void MatWorldCompose(OUT _matrix& matOut, const _vec3& vScale, const D3DXQUATERNION& qRot, const _vec3& vPos);
	static void MatWorldComposeEuler(OUT _matrix& matOut, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos);
	static void MatWorldDecompose(const _matrix& matWorld, OUT _vec3& vScale, OUT _vec3& vAngle, OUT _vec3& vPos);
	static void GetRandomVector(_vec3* out, _vec3* min, _vec3* max);
	static void RemoveScale(_matrix& matOut);
	static _float Vec3LenXZ(const _vec3& v1);
	static _float GetRandomFloat(_float lowBound, _float highBound);
	static void GetPickingRay(OUT _vec3& vOrigin, OUT _vec3& vRayDir, HWND hWnd, const _matrix& matView, const _matrix& matProj, const D3DVIEWPORT9& ViewPort);
	static void World2Screen(OUT _vec2& vScreen, const _vec3& vPos, const _matrix& matView, const _matrix& matProj, const D3DVIEWPORT9& ViewPort);
	static _float FloatLerp(_float f1, _float f2, _float fS);
	static DWORD FtoDw(_float f);
	static const _vec3 s_vZero;
	static const _vec3 s_vOne;

	static const _vec3 s_vUp;
	static const _matrix s_matIdentity;
	static const _vec3 s_vFaceCubeVtx[FACE_END][4];

	static _float s_fTimeDelta;
};

END