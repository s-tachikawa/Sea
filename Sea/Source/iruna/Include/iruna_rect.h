//---------------------------------------------------------
// 
//	iruna_rect.h
//	RECT型関連
//	作成日 6月24日
//	製作者 立川 翔野
// 
//---------------------------------------------------------

/*----多重インクルード防止-------------------------------------------------*/
#ifndef IRUNA_RECT_H
#define IRUNA_RECT_H

/*----インクルード-------------------------------------------------*/
#include "../Include/iruna_utility.h"

/*----データ型 宣言------------------------------------------------*/
namespace iruna
{
namespace math
{

//=====================================================================
//
//	構造体名	:	Rect
//	処理		:	矩形型の処理
//	製作者		:	立川 翔野
//	作成日		:	2011/06/24	- 仮作成
//
//=====================================================================
struct Rect
{
	int		left;
	int		top;
	int		right;
	int		bottom;

	Rect( int setLeft = 0, int setTop = 0, int setRight = 0, int setBottom = 0 )
		: left( setLeft ), top( setTop ), right( setRight ), bottom( setBottom ) {}
};

//=====================================================================
//
//	構造体名	:	RectF
//	処理		:	矩形型の処理	
//	製作者		:	立川 翔野
//	作成日		:	2011/06/24	- 仮作成
//
//=====================================================================
struct RectF
{
	float	left;
	float	top;
	float	right;
	float	bottom;

		RectF( float setLeft = 0, float setTop = 0, float setRight = 0, float setBottom = 0 )
		: left( setLeft ), top( setTop ), right( setRight ), bottom( setBottom ) {}
};

}	//	math	end
}	//	iruna	end

#endif	//	IRUNA_RECT_H	end
