//---------------------------------------------------------
// 
//	iruna_types.h
//	型定義
//	作成日 6月22日
//	製作者 立川 翔野
// 
//---------------------------------------------------------

/*----多重インクルード防止-------------------------------------------------*/
#ifndef IRUNA_TYPES_H
#define IRUNA_TYPES_H

/*----インクルード-------------------------------------------------*/
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include <xaudio2.h>

// dinput.hを読み込む前に宣言しなければならない
#ifndef DIRECTINPUT_VERSION
	#define	DIRECTINPUT_VERSION	0x0800
#endif
#include <dinput.h>

/*----データ型 宣言------------------------------------------------*/
namespace iruna
{

typedef unsigned char				U_CHAR;
typedef unsigned short				U_SHORT;
typedef unsigned int				U_INT;
typedef unsigned long				U_LONG;

} // iruna end


#endif // IRUNA_TYPES_H