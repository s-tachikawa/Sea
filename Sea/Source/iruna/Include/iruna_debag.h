//---------------------------------------------------------
// 
//	iruna_debag.h
//	�f�o�b�O�p�֐�
//	�쐬�� 6��22��
//	����� ���� �Ė�
// 
//---------------------------------------------------------

/*----���d�C���N���[�h�h�~-------------------------------------------------*/
#ifndef IRUNA_DEBUG_H
#define IRUNA_DEBUG_H

/*----�C���N���[�h-------------------------------------------------*/
#include "../Include/iruna_types.h"

/*----�֐� ��`----------------------------------------------------*/
namespace iruna
{

//--------------------------------------------------------------------
// 
//	����		:	�f�o�b�O�E�B���h�E�ɕ����Ɛ��l���o�͂���(�f�o�b�O���̂ݗL��)
//	��1����		:	[in] str	- �o�͂��镶����
//	��2����		:	[in] ...	- �Ԃ����݂������l
//	�����		:	���� �Ė�
//	�쐬��		:	2011/06/22	- �쐬
// 
//--------------------------------------------------------------------
inline void DrawDebugFormatString( LPCSTR str, ... )
{
	va_list		list;
	char		temp[ 256 ];

	va_start( list, str );
	vsprintf_s( temp, 256, str, list );
	va_end( list );
	
	OutputDebugString( temp );

	return;
}

}	// iruna	end
#endif	// IRUNA_DEBUG_H	end