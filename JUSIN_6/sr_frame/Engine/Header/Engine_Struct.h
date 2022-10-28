#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef	struct tagVertexColor
	{
		_vec3		vPos;
		_ulong		dwColor;

	}VTXCOL;

	const _ulong		FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTex
	{
		_vec3		vPos;
		_vec3		vNormal;
		_vec2		vTexUV;
	} VTXTEX;
	const _ulong		FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef	struct tagIndex16
	{
		_ushort			_0;
		_ushort			_1;
		_ushort			_2;

	}INDEX16;

	typedef	struct tagIndex32
	{
		_ulong		_0;
		_ulong		_1;
		_ulong		_2;

	}INDEX32;

	typedef struct tagVertexCubeTex
	{
		_vec3		vPos;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong		FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);		// D3DFVF_TEXCOORDSIZE3 : 텍스처의 UV값이 FLOAT 형 3개의 크기만큼이며, 괄호산의 숫자 0의 의미는 본래 버텍스에 텍스처 UV 값이 여러개가 올 수 있는데 그중 0번째 것이 값을 지정하겠다는 의미이다.


	typedef struct tagVertexCubeTex2
	{
		_vec3		vPos;
		_vec2		vTexUV;

	}VTXCUBE2;

	const _ulong		FVF_CUBE2 = D3DFVF_XYZ | D3DFVF_TEX1;

	struct VertexCubeNormal
	{
		_vec3 vPos;
		_vec3 vNormal;
		_vec3 vTexUV;
	};
	const _ulong		FVF_CUBE_NOR = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);		// D3DFVF_TEXCOORDSIZE3 : 텍스처의 UV값이 FLOAT 형 3개의 크기만큼이며, 괄호산의 숫자 0의 의미는 본래 버텍스에 텍스처 UV 값이 여러개가 올 수 있는데 그중 0번째 것이 값을 지정하겠다는 의미이다.

	struct VertexCubeNormal2
	{
		_vec3 vPos;
		_vec3 vNormal;
		_vec2 vTexUV;
	};
	const _ulong		FVF_CUBE_NOR2 = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 ;


	//MapImGuiTool
	typedef struct maptool
	{
		_int	iTexIdx;
		_int	iPlantIdx;
		_int	iCubeCount;
		_int	iPickingOption;
		_float	fHeight;
		_int	iCubeType;
		_bool	bRendState;

	}MapTool;
#pragma region Particle Struct
	

	

	struct Attribute
	{
		Attribute()
		{
			_fLifeTime = 0.0f;
			_fAge = 0.0f;
			_bIsAlive = true;
		}

		_vec3		_vPosition;	//	월드 스페이스 내의 파티클 위치 
		_vec3		_vVelocity;  //  파티클의 속도, 보통은 초당 이동단위로 기록  
		_vec3		_vAcceleration;  // 파티클의 가속, 보통은 초당 이동단위로 기록
		_float		_fLifeTime;     // 파티클이 소멸할 때 까지 유지되는 시간
		_float      _fAge;          // 파티클의 현재 나이
		D3DXCOLOR   _color;        // 파티클의 색상
		D3DXCOLOR   _colorFade;    // 파티클의 컬러가 시간의 흐름에 따라 퇴색하는 방법
		_bool       _bIsAlive;	   // 파티클이 생존한 경우 True, 소멸한 경우 False
		_float		_fSize;
		_int		_iType;
	};
#pragma endregion



#pragma region Attack_Range
	enum ATTACKCIRCLETYPE
	{
		READY_CIRCLE = 0,
		ATTACK_CIRCLE
	};
	typedef struct AttackRangeOption
	{
		_float _fAcc;
		_float _fMaxAcc;

		_float _fLifeTime;
		_float _fTime;

		_vec3 _vMinSize;
		_vec3 _vMaxSize;

		_uint _iTotalFrame;
		_uint _iNextFrame;
		_vec3 _vLerpScale;

		ATTACKCIRCLETYPE _eRangeType;
	}ATKRNGOPTION;


#pragma endregion
}



#endif // Engine_Struct_h__
