#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3	vPos;
		_ulong	dwColor;

	}VTXCOL;
	// ������ ����
	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;
								// POS	 |  ����		|	�ؽ�ó�� �������� �ʴ´�

	typedef struct tagVertexTex
	{
		_vec3	vPos;
		_vec3	vNormal;
		_vec2	vTexUV;	// UV��ǥ : �ؽ��ĸ� ���� ������ǥ(����)
						// 0,0 ~ 1,1
	}VTXTEX;

	const _ulong		FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
											// D3DFVF_NORMAL : ���� ������ ���� ���� ���͸� ����

	typedef struct tagVertexCubeTex
	{
		_vec3	vPos;
		_vec3	vTexUV;
	}VTXCUBE;
	
	const _ulong		FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);		
	// D3DFVF_TEXCOORDSIZE3 : �ؽ�ó�� UV���� FLOAT �� 3���� ũ�⸸ŭ�̸�,
	// ��ȣ���� ���� 0�� �ǹ̴� ���� ���ؽ��� �ؽ�ó UV ���� �������� �� �� �ִµ� 
	// ���� 0��° ���� ���� �����ϰڴٴ� �ǹ��̴�.

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


	// ��ƼŬ
	typedef struct Particle
	{
		_vec3		_position;	// ��ġ
		D3DCOLOR	_color;		// �÷�
		//_float			_size;		// ũ��
	}PARTICLE;
	const _ulong		FVF_PARTICLE = D3DFVF_XYZ | D3DFVF_DIFFUSE; // D3DFVF_PSIZE : ũ�������� : �׷���ī�忡 ���� ���������� ������.
								//D3DFVF_XYZ : ��ȯ���� ���� ������ ��ġ�� ���Ե�.
								//D3DFVF_DIFFUSE : Ȯ��� ������Ұ� ���Ե�.

	// ��ƼŬ �Ӽ�
	typedef struct Attribute
	{
		_vec3		_position;		// ���彺���̽� �� ��ƼŬ ��ġ
		_vec3		_velocity;		// ��ƼŬ �ӵ�(���� /s)
		_vec3		_acceleration;	// ��ƼŬ�� ����(���� /s)
		_float		_lifeTime;		// ��ƼŬ �Ҹ���� �����Ǵ� �ð�.
		_float		_age;			// ��ƼŬ�� ���� ����
		D3DXCOLOR	_color;			// ��ƼŬ�� �÷�
		D3DXCOLOR	_colorFade;		// ��ƼŬ�� �÷��� �ð��� �v�� ���� ����ϴ� ���
		_bool		_isAlive;		// ��ƼŬ�� ������ ��� True, �Ҹ��� ��� False

	}ATTRIBUTE;
}

#endif // Engine_Struct_h__
