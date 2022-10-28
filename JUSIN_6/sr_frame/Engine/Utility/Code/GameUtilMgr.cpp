#include "GameUtilMgr.h"

USING(Engine);

_float w2 = 0.5f;
_float h2 = 0.5f;
_float d2 = 0.5f;

const _vec3 CGameUtilMgr::s_vZero = {0.f, 0.f, 0.f};
const _vec3 CGameUtilMgr::s_vOne = { 1.f, 1.f, 1.f };

const _vec3 CGameUtilMgr::s_vUp = {0.f, 1.f, 0.f};
const _matrix CGameUtilMgr::s_matIdentity = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f,};
const _vec3 CGameUtilMgr::s_vFaceCubeVtx[FACE_END][4]{
	{// look
		{-w2, -h2, +d2},
		{+w2, -h2, +d2},
		{+w2, +h2, +d2},
		{-w2, +h2, +d2}
	},// back
	{
		{-w2, -h2, -d2},
		{-w2, +h2, -d2},
		{+w2, +h2, -d2},
		{+w2, -h2, -d2},
	},
	{// left
		{-w2, -h2, +d2},
		{-w2, +h2, +d2},
		{-w2, +h2, -d2},
		{-w2, -h2, -d2}
	},
	{//right
		{+w2, -h2, -d2},
		{+w2, +h2, -d2},
		{+w2, +h2, +d2},
		{+w2, -h2, +d2}
	},
	{// up
		{-w2, +h2, -d2},
		{-w2, +h2, +d2},
		{+w2, +h2, +d2},
		{+w2, +h2, -d2}
	},
	{// down
		{-w2, -h2, -d2},
		{+w2, -h2, -d2},
		{+w2, -h2, +d2},
		{-w2, -h2, +d2},
	}
};
_float CGameUtilMgr::s_fTimeDelta = 0.f;

void CGameUtilMgr::WorldMatrixLerp(_matrix& matOut, const _matrix& matPrev, const _matrix& matNext, const _float fS)
{
	_vec3 vPrevPos, vNextPos; // pos
	_vec3 vPrevScale, vNextScale; // scale
	D3DXQUATERNION qPrevQuat, qNextQuat; // rot
	D3DXMatrixDecompose(&vPrevScale, &qPrevQuat, &vPrevPos, &matPrev);
	D3DXMatrixDecompose(&vNextScale, &qNextQuat, &vNextPos, &matNext);
	D3DXQuaternionNormalize(&qPrevQuat, &qPrevQuat);
	D3DXQuaternionNormalize(&qNextQuat, &qNextQuat);

	_vec3 vLerpPos = vPrevPos;
	_vec3 vLerpScale = vPrevScale;
	D3DXQUATERNION qLerpQuat = qPrevQuat;
	
	if (Vec3Cmp(vPrevPos, vNextPos) == false)
		D3DXVec3Lerp(&vLerpPos, &vPrevPos, &vNextPos, fS);

	if (Vec3Cmp(vPrevScale, vNextScale) == false)
		D3DXVec3Lerp(&vLerpScale, &vPrevScale, &vNextScale, fS);

	if (QuatCmp(qPrevQuat, qNextQuat) == false)
		D3DXQuaternionSlerp(&qLerpQuat, &qPrevQuat, &qNextQuat, fS);

	_matrix matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, vLerpScale.x, vLerpScale.y, vLerpScale.z);
	D3DXMatrixRotationQuaternion(&matRot, &qLerpQuat);
	D3DXMatrixTranslation(&matTrans, vLerpPos.x, vLerpPos.y, vLerpPos.z);

	matOut = matScale * matRot * matTrans;
}

void CGameUtilMgr::QuatToPitchYawRoll(const D3DXQUATERNION& q, _float& pitch, _float& yaw, _float& roll)
{
	// to radian version
	_float sqw = q.w * q.w;
	_float sqx = q.x * q.x;
	_float sqy = q.y * q.y;
	_float sqz = q.z * q.z;

	pitch = asinf(2.f * (q.w * q.x - q.y * q.z));
	yaw = atan2f(2.f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));
	roll = atan2f(2.f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));
}

bool CGameUtilMgr::Vec3Cmp(const _vec3& v1, const _vec3& v2, _float fEpsilon)
{
	return fabs(v1.x - v2.x) < fEpsilon
		&& fabs(v1.y - v2.y) < fEpsilon
		&& fabs(v1.z - v2.z) < fEpsilon;
}

bool CGameUtilMgr::QuatCmp(const D3DXQUATERNION& q1, const D3DXQUATERNION& q2, _float fEpsilon)
{
	return fabs(q1.x - q2.x) < fEpsilon
		&& fabs(q1.y - q2.y) < fEpsilon
		&& fabs(q1.z - q2.z) < fEpsilon
		&& fabs(q1.w - q2.w) < fEpsilon;
}

bool CGameUtilMgr::MatCmp(const _matrix& m1, const _matrix& m2, _float fEpsilon)
{
	for (_uint i = 0; i < 4; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			if (fabs(m1(i, j) - m2(i, j)) >= fEpsilon)
			{
				return false;
			}
		}
	}

	return true;
}

bool CGameUtilMgr::FloatCmp(const _float& f1, const _float f2, _float fEpsilon)
{
	return fabs(f1 - f2) < fEpsilon;
}

void CGameUtilMgr::MatWorldCompose(_matrix& matOut, const _vec3& vScale, const D3DXQUATERNION& qRot,
                                   const _vec3& vPos)
{
	_matrix matScale;
	D3DXMatrixIdentity(&matOut);
	D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixRotationQuaternion(&matOut, &qRot);

	matOut = matScale * matOut;

	matOut._41 = vPos.x;
	matOut._42 = vPos.y;
	matOut._43 = vPos.z;
}

void CGameUtilMgr::MatWorldComposeEuler(_matrix& matOut, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	D3DXMatrixIdentity(&matOut);

	_matrix matRot[3];

	matOut._11 *= vScale.x;
	matOut._22 *= vScale.y;
	matOut._33 *= vScale.z;

	D3DXMatrixRotationX(&matRot[0], vAngle.x);
	D3DXMatrixRotationY(&matRot[1], vAngle.y);
	D3DXMatrixRotationZ(&matRot[2], vAngle.z);

	matOut = matOut * matRot[0] * matRot[1] * matRot[2];

	matOut._41 = vPos.x;
	matOut._42 = vPos.y;
	matOut._43 = vPos.z;
}

void CGameUtilMgr::MatWorldDecompose(const _matrix& matWorld, _vec3& vScale, _vec3& vAngle, _vec3& vPos)
{
	D3DXQUATERNION qRot;
	D3DXMatrixDecompose(&vScale, &qRot, &vPos, &matWorld);
	QuatToPitchYawRoll(qRot, vAngle.x, vAngle.y, vAngle.z);
}

void CGameUtilMgr::GetRandomVector(_vec3* out, _vec3* min, _vec3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

void CGameUtilMgr::RemoveScale(_matrix& matOut)
{
	D3DXVec3Normalize((_vec3*)(&matOut[0]), (_vec3*)(&matOut[0]));
	D3DXVec3Normalize((_vec3*)(&matOut[1]), (_vec3*)(&matOut[1]));
	D3DXVec3Normalize((_vec3*)(&matOut[2]), (_vec3*)(&matOut[2]));
}

_float CGameUtilMgr::Vec3LenXZ(const _vec3& v1)
{
	return sqrtf(v1.x * v1.x + v1.z * v1.z);
}

_float CGameUtilMgr::GetRandomFloat(_float lowBound, _float highBound)
{
	if (lowBound >= highBound) 
		return lowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (highBound - lowBound)) + lowBound;
}

void CGameUtilMgr::GetPickingRay(_vec3& vOrigin, _vec3& vRayDir, HWND hWnd, const _matrix& matView,
	const _matrix& matProj, const D3DVIEWPORT9& ViewPort)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3 vAt;

	vOrigin.x = (_float)ptMouse.x / ((_float)ViewPort.Width * 0.5f) - 1.f;
	vOrigin.y = (_float)ptMouse.y / -((_float)ViewPort.Height * 0.5f) + 1.f;
	vOrigin.z = 0.f;
	vAt.x = vOrigin.x;
	vAt.y = vOrigin.y;
	vAt.z = 1.f;

	_matrix matProjInverse;
	D3DXMatrixInverse(&matProjInverse, nullptr, &matProj);
	D3DXVec3TransformCoord(&vOrigin, &vOrigin, &matProjInverse);
	D3DXVec3TransformCoord(&vAt, &vAt, &matProjInverse);

	_matrix matViewInverse;
	D3DXMatrixInverse(&matViewInverse, nullptr, &matView);
	D3DXVec3TransformCoord(&vOrigin, &vOrigin, &matViewInverse);
	D3DXVec3TransformCoord(&vAt, &vAt, &matViewInverse);

	vRayDir = vAt - vOrigin;
	D3DXVec3Normalize(&vRayDir, &vRayDir);
}

void CGameUtilMgr::World2Screen(_vec2& vScreen, const _vec3& vPos, const _matrix& matView, const _matrix& matProj,
	const D3DVIEWPORT9& ViewPort)
{
	const _matrix matViewProj = matView * matProj;

	_vec3 vPosToScreen;
	D3DXVec3TransformCoord(&vPosToScreen, &vPos, &matViewProj);

	vPosToScreen.x = (_float)ViewPort.Width * ( vPosToScreen.x + 1.0f ) / 2.0f + (_float)ViewPort.X;
	vPosToScreen.y = (_float)ViewPort.Height * ( 2.0f - ( vPosToScreen.y + 1.0f ) ) / 2.0f + (_float)ViewPort.Y;

	vScreen = {vPosToScreen.x, vPosToScreen.y};
}

_float CGameUtilMgr::FloatLerp(_float f1, _float f2, _float fS)
{
	return f1 + fS * (f2 - f1);
}

DWORD CGameUtilMgr::FtoDw(_float f)
{
	return *((DWORD*)&f);
}
