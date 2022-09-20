#pragma once

#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CPipeline
{
public:
	explicit CPipeline();
	~CPipeline();

public:
	static	void MakeTransformMatrix(_matrix* pMatrix, const _vec3* pRight, const _vec3* pUp, const _vec3* pLook, const _vec3* pPos);
	static	void MakeViewMatirx(_matrix* pMatrix, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	static	void MakeProjMatirx(_matrix* pMatrix, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar);


};

END