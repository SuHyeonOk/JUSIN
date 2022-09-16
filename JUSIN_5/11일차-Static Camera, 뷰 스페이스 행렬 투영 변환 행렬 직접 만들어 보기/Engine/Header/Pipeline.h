#pragma once

#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CPipeline
{
public:
	explicit CPipeline();
	~CPipeline();

public:
	// ����� ����� �Լ�
	static	void MakeTransformMatrix(_matrix* pMatrix, const _vec3* pRight, const _vec3* pUp, const _vec3* pLook, const _vec3* pPos);
	// �� �����̽� ��ȯ ���
	static	void MakeViewMatirx(_matrix* pMatrix, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	// ���� ��ȯ ���
	static	void MakeProjMatirx(_matrix* pMatrix, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar);


};

END