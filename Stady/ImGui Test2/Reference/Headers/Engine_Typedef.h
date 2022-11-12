#pragma once

namespace Engine
{
	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

	typedef		unsigned char			_uchar;
	typedef		signed char				_char;
	typedef		wchar_t					_tchar;

	typedef		signed char				_byte;
	typedef		unsigned char			_ubyte;

	typedef		signed short			_short;
	typedef		unsigned short			_ushort;

	typedef		signed int				_int;
	typedef		unsigned int			_uint;

	typedef		signed long				_long;
	typedef		unsigned long			_ulong;

	typedef		float					_float;
	typedef		double					_double;

	typedef		bool					_bool;

	typedef		XMFLOAT2				_float2;
	typedef		XMFLOAT3				_float3;
	typedef		XMFLOAT4				_float4;
	typedef		XMFLOAT4X4				_float4x4;
}