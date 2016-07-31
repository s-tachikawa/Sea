//---------------------------------------------------------
// 
//	iruna_bord.h
//	板ポリのデフォルトクラス(派生してビルボードとかにする)
//	作成日 6月24日
//	製作者 立川 翔野
// 
//---------------------------------------------------------

/*----多重インクルード防止-------------------------------------------------*/
#ifndef IRUNA_BORD_H
#define IRUNA_BORD_H

/*----インクルード-------------------------------------------------*/
#include "../Include/iruna_utility.h"
#include "../Include/iruna_math.h"
#include "../Include/iruna_color4.h"
#include "../Include/iruna_vertex_declaration.h"
#include "../Include/iruna_vertex_buffer.h"
#include "../Include/iruna_device_controller.h"

/*----データ型 宣言------------------------------------------------*/
namespace iruna
{
namespace graphics
{

//=====================================================================
//
//	クラス名	:	Board
//	処理		:	板ポリゴンクラス
//	製作者		:	立川 翔野
//	作成日		:	2011/06/24	- 作成
//
//=====================================================================
class Board
{
//	プロテクテッド	メンバ変数
protected:
	VertexBuffer*				positionBuffer;	// 頂点の座標バッファ
	VertexBuffer*				texcoordBuffer;	// 頂点のuv座標バッファ
	VertexBuffer*				colorBuffer;	// 頂点の色バッファ
	math::Vector3				size[ 4 ];		// バックアップ用のボードサイズ
	math::Vector2				texcoord[ 4 ];	// バックアップ用のuv座標
	Color4						color[ 4 ];		// バックアップ用の色
	DeviceController*			render;			// 間接的にデバイスを操作する
	static VertexDeclaration	declaration;	// ボードの頂点宣言(静的メンバ変数)

//	パブリック	メンバ関数
public:
	//--------------------------------------------------------------------
	// 
	//	処理	:	コンストラクタ
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	Board();

	//--------------------------------------------------------------------
	// 
	//	処理	:	デストラクタ(仮想関数)
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	virtual	~Board();

	//--------------------------------------------------------------------
	// 
	//	処理	:	板ポリゴンのサイズを設定する(仮想関数)
	//	第1引数	:	[in] setSize	- 板ポリゴンのサイズ
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	virtual	void	SetSize( math::Vector2 setSize );

	//--------------------------------------------------------------------
	// 
	//	処理	:	板ポリゴンにテクスチャのどの部分を貼るか(仮想関数)
	//	第1引数	:	[in] uvRect	- テクスチャの部分矩形
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	virtual void	SetTexUV( math::RectF uvRect );

	//--------------------------------------------------------------------
	// 
	//	処理	:	板ポリゴンの色を設定する(仮想関数)
	//	第1引数	:	[in] setColor	- 板ポリゴンの色
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	virtual void	SetColor( Color4 setColor );

	//--------------------------------------------------------------------
	// 
	//	処理	:	板ポリゴンを表示する(仮想関数)
	//	戻り値	:	結果フラグ	ture：成功　false：失敗
	//	製作者	:	立川 翔野
	//	作成日	:	2011/06/24	- 作成
	// 
	//--------------------------------------------------------------------
	virtual bool	Draw();

};

}	//	graphics	end
}	//	iruna		end

#endif	//	IRUNA_BORD_H	end