#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3	vPos;
		_ulong	dwColor;

	}VTXCOL;
	// 꼭짓점 형식
	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;
								// POS	 |  색상		|	텍스처를 참조하지 않는다

	typedef struct tagVertexTex
	{
		_vec3	vPos;
		_vec3	vNormal;
		_vec2	vTexUV;	// UV좌표 : 텍스쳐만 갖는 고유좌표(비율)
						// 0,0 ~ 1,1
	}VTXTEX;

	const _ulong		FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
											// D3DFVF_NORMAL : 정점 포맷이 정점 법선 벡터를 포함

	typedef struct tagVertexCubeTex
	{
		_vec3	vPos;
		_vec3	vTexUV;
	}VTXCUBE;
	
	const _ulong		FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);		
	// D3DFVF_TEXCOORDSIZE3 : 텍스처의 UV값이 FLOAT 형 3개의 크기만큼이며,
	// 괄호산의 숫자 0의 의미는 본래 버텍스에 텍스처 UV 값이 여러개가 올 수 있는데 
	// 그중 0번째 것이 값을 지정하겠다는 의미이다.

	typedef struct tagIndex16
	{
		_ushort		_0;
		_ushort		_1;
		_ushort		_2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong		_0;
		_ulong		_1;
		_ulong		_2;

	}INDEX32;


	// 파티클
	typedef struct Particle
	{
		_vec3		_position;	// 위치
		D3DCOLOR	_color;		// 컬러
		//_float			_size;		// 크기
	}PARTICLE;
	const _ulong		FVF_PARTICLE = D3DFVF_XYZ | D3DFVF_DIFFUSE; // D3DFVF_PSIZE : 크기제어요소 : 그래픽카드에 따라 지원유무가 갈린다.
								//D3DFVF_XYZ : 변환되지 않은 꼭짓점 위치가 포함됨.
								//D3DFVF_DIFFUSE : 확산색 구성요소가 포함됨.

	// 파티클 속성
	typedef struct Attribute
	{
		_vec3		_position;		// 월드스페이스 내 파티클 위치
		_vec3		_velocity;		// 파티클 속도(보통 /s)
		_vec3		_acceleration;	// 파티클의 가속(보통 /s)
		_float		_lifeTime;		// 파티클 소멸까지 유지되는 시간.
		_float		_age;			// 파티클의 현재 나이
		D3DXCOLOR	_color;			// 파티클의 컬러
		D3DXCOLOR	_colorFade;		// 파티클의 컬러가 시간의 흚에 따라 퇴색하는 방법
		_bool		_isAlive;		// 파티클이 생존한 경우 True, 소멸한 경우 False

	}ATTRIBUTE;
}

#endif // Engine_Struct_h__
