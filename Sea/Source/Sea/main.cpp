
#include <iruna.h>

using namespace iruna;
using namespace	graphics;
using namespace math;
using namespace io;
using namespace core;

int WINAPI WinMain( HINSTANCE , HINSTANCE, LPTSTR, int )
{

	// ライブラリ管理クラス
	// ライブラリの起動、一番最初にする
	System	system	= System::GetInstance();
	if( !system.Setup( 800, 600, true,  "海を作ってみた"  ) )	// WS_POPUPで枠が消える
		return -1;

	float				cameraAngle	= 0.f;
	float				cameraUp	= 4.0f;
	float				length		= D3DX_PI / cameraUp;
	// カメラの準備
	Camera			camera;
	camera.eye		= math::Vector3( 0.f, 220.f, 250.f );
	camera.lookAt	= math::Vector3( 0.f, 0.f, -30.f );
	camera.upVector	= math::Vector3( 0.f, 1.f, 0.f );
	camera.fovY		= length;
	camera.aspect	= 800.f / 600.f;
	camera.zNear	= 0.1f;
	camera.zFar		= 500.f;



	// 頂点宣言Xファイル用
	static VertexDeclaration::Element VertexElement[] = {
		// 頂点
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
		// uv座標
		{ 1, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	
	};

	math::Matrix wold;
	D3DXMatrixTranslation( &wold , 0.0f , 0.0f , 0.0f );
	DeviceController render;

	// モデルのロード
	Model* model = new Model;
	model->Load( "../Resource/model/Sea.x" );
	model->RemakeVertexFormat( VertexElement );

	// エフェクトのロード
	Effect* nomal	= new Effect;
	Effect* wave		= new Effect;
	Effect* sea		= new Effect;
	nomal->Create( "../Resource/efect/CreateNormal.fx",1 );
	wave->Create( "../Resource/efect/UpdateHeightVelocity.fx",1  );
	sea->Create(  "../Resource/efect/Sea.fx",1  );

	
	// バックバッファサーフェース
	Surface* backBuffer			= new Surface;
	// Zバッファ
	Surface* ZBuffer			= new Surface;
	

	// 波の基準テクスチャー
	Texture* waveTex			= new Texture;
	waveTex->Load( "../Resource/texture/Wave.dds" );

	// 高さマップの読み込み
	Texture* hightTex			= new Texture[ 2 ];
	hightTex[ 0 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );
	hightTex[ 1 ].Load( "../Resource/texture/Height.dds" );

	// 高さマップのサーフェース取得
	Surface* hightRtTex			= new Surface[ 2 ];
	hightTex[ 0 ].GetSurface( hightRtTex[ 0 ] );
	hightTex[ 1 ].GetSurface( hightRtTex[ 1 ] );

	// 波の速さマップの読み込み
	Texture* velocityTex		= new Texture[ 2 ];
	velocityTex[ 0 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );
	velocityTex[ 1 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );

	// 速さマップのサーフェース取得
	Surface* velocityRtTex		= new Surface[ 2 ];
	velocityTex[ 0 ].GetSurface( velocityRtTex[ 0 ] );
	velocityTex[ 1 ].GetSurface( velocityRtTex[ 1 ] );

	// 法泉マップ
	Texture* nomalTex			= new Texture;
	nomalTex->Create( 256,256 , D3DUSAGE_RENDERTARGET );

	// 法泉マップのサーフェースの取得
	Surface* nomalRtTex			= new Surface;
	nomalTex->GetSurface( (*nomalRtTex) );


	// 環境マップの読み込み
	CubeTexture* cubeTex = new CubeTexture;
	cubeTex->Load( "../Resource/texture/Environment.dds" );



	// テクスチャーのオフセット(テクスチャーの上下左右を取得するためのもの)
	math::Vector4 waveTexOffset = math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
	math::Vector4 waveTexSize = math::Vector4( 256.0f, 256.0f, 0.0f, 0.0f );
	// 波テクスチャーのインデックス
	int heightMapIndex = 1;

	// ライトの計算
	float lightVec = 0.01f;
	// 平行光源用ライトベクトル
	math::Vector4 directionLightVector	= math::Vector4( 0.5f, lightVec, 0.5f, 0.0f );

	// 平行光源用ライト色
	math::Vector4 directionLightColor	= math::Vector4( 0.4f, 0.4f, 0.6f, 0.0f );
	// スペキュラー色
	math::Vector4 specularLightColor	= math::Vector4( 1.2f, 0.6f, 0.2f, 0.0f );


	// 頂点バッファ,頂点宣言作成
	VertexBuffer* vertex = new VertexBuffer[ 2 ];
	math::Vector4* pos = new math::Vector4[ 4 ];
	math::Vector2* uv = new math::Vector2[ 4 ];

	VertexDeclaration* dec = new VertexDeclaration;

	static VertexDeclaration::Element Vertex2DElement[] = {
		// 頂点
		{ 0, 0, D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
		// uv座標
		{ 1, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	
	};
	dec->Create( Vertex2DElement );
	
	float fOffsetX = 0.5f / 800.0f;
	float fOffsetY = 0.5f / 600.0f;

	pos[ 0 ].x	= -1.0f;
	pos[ 0 ].y	= +1.0f;
	pos[ 0 ].z	= 0.0f;
	pos[ 0 ].w	= 1.0f;
	uv[ 0 ].x	= 0.0f;
	uv[ 0 ].y	= 0.0f;

	pos[ 1 ].x	= +1.0f;
	pos[ 1 ].y	= +1.0f;
	pos[ 1 ].z	= 0.0f;
	pos[ 1 ].w	= 1.0f;
	uv[ 1 ].x	= 1.0f;
	uv[ 1 ].y	= 0.0f;

	pos[ 2 ].x	= -1.0f;
	pos[ 2 ].y	= -1.0f;
	pos[ 2 ].z	= 0.0f;
	pos[ 2 ].w	= 1.0f;
	uv[ 2 ].x	= 0.0f;
	uv[ 2 ].y	= 1.0f;

	pos[ 3 ].x	= +1.0f;
	pos[ 3 ].y	= -1.0f;
	pos[ 3 ].z	= 0.0f;
	pos[ 3 ].w	= 1.0f;
	uv[ 3 ].x	= 1.0f;
	uv[ 3 ].y	= 1.0f;

	vertex[ 0 ].Create( VertexBuffer::FLOAT4, 4 );
	vertex[ 1 ].Create( VertexBuffer::FLOAT2, 4 );

	vertex[ 0 ].SetBuffer( pos, 0, 4 );
	vertex[ 1 ].SetBuffer( uv, 0, 4 );


	// スプライト
	Sprite* sprite = new Sprite;
	sprite->SetSize( math::Vector2( 256,256 ) );

	// 背景
	Sprite* back = new Sprite;
	back->SetSize( math::Vector2( 800,600 ) );
	Texture* backTex = new Texture;
	backTex->Load("../Resource/texture/14656967_m.jpg"  );
	back->SetColor( Color4( 255,255,255,255 ) );
	
	sound::StreamSound sound;
	sound.LoadSound( "../Resource/sound/sea.ogg" );
	sound.PlaySound();
	sound.SetVolume( 0.5f );
	// メインループ
	while( system.Run() )
	{
	
		// ウィンドウがアクティブの時だけ実行
		if( system.IsActive() )
		{

			//	キーの処理(かめらの更新)
			if( io::CheckKeyState( DIK_LEFT ) )
				cameraAngle += 0.01f;
			if( io::CheckKeyState( DIK_RIGHT ) )
				cameraAngle -= 0.01f;
			
			// ライトの更新
			if( io::CheckKeyState( DIK_SPACE ) )
			{
				if( io::CheckKeyState( DIK_UP ) )
				{
					cameraUp += 0.05f;
					length = D3DX_PI / cameraUp;
					camera.fovY = length;
				}
				if( io::CheckKeyState( DIK_DOWN ) )
				{
					cameraUp -= 0.05f;
					length = D3DX_PI / cameraUp;
					camera.fovY = length;
				}
			}
			else
			{
				if( io::CheckKeyState( DIK_UP ) )
					lightVec += 0.01f;
				if( io::CheckKeyState( DIK_DOWN ) )
					lightVec -= 0.01f;
			}
				

			math::Vector4 directionLightVector	= math::Vector4( 0.5f, lightVec, 0.5f, 0.0f );

			D3DXMatrixTranslation(&wold , 0.0f , 0.0f , 0.0f );
			D3DXMatrixRotationYawPitchRoll( &wold, cameraAngle, 0.f, 0.f );
			
			// 波の更新と描画

			//	波テクスチャのUVオフセット値の更新
			waveTexOffset.x += 0.0010f;
			waveTexOffset.y -= 0.0015f;
			// UV座標なので1.0を超えないようにする
			while ( waveTexOffset.x < 0.0f ) waveTexOffset.x += 1.0f;
			while ( waveTexOffset.y < 0.0f ) waveTexOffset.y += 1.0f;


			ClearScreen();
			// バックバッファをとっておく
			GetBackBuffer( *backBuffer );
			Viewport	oldViewport;
			Viewport	newViewport;
			GetViewport( oldViewport );
			newViewport.SetSize( 0, 0, static_cast< int >( 256 ), static_cast< int >( 256 ) );

			// 波マップと高さマップの更新
			velocityRtTex[ heightMapIndex ].SetRenderTarget( 1 );
			hightRtTex[ heightMapIndex ].SetRenderTarget( 0 );
			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
			

			// Zバッファは使わない
			render.SetZBufferState( false );
			Surface		oldDepthTarget;
			Surface		newDepthTarget;
			GetDepthTarget( oldDepthTarget );
			newDepthTarget.SetDepthTarget();	//空サーフェスで無効化newViewport.SetViewport();


			// テクスチャのオフセットテーブル
			math::Vector4 TextureOffsetTbl[] = {
				math::Vector4(-1.0f,  0.0f, 0.0f, 0.0f),
				math::Vector4(+1.0f,  0.0f, 0.0f, 0.0f),
				math::Vector4( 0.0f, -1.0f, 0.0f, 0.0f),
				math::Vector4( 0.0f, +1.0f, 0.0f, 0.0f),
			};
			// テクスチャーのサイズに合わせる
			for ( int i = 0; i < 4; i++ )
			{
				TextureOffsetTbl[i].x /= waveTexSize.x;
				TextureOffsetTbl[i].y /= waveTexSize.y;
			}
			wave->SetTexture( "g_TextureHeight", hightTex[ 1 - heightMapIndex ].GetTexture() );
			wave->SetTexture( "g_TextureVelocity", velocityTex[ 1 - heightMapIndex ].GetTexture() );
			wave->SetVectorArayy( "g_TextureOffsetTbl", TextureOffsetTbl, 4 );
			wave->SetFloat( "g_WaveSpeed", 0.01f );

			// 値が変更したことをデバイスに伝える
			wave->CommitChanges();

			dec->Set();
			vertex[ 0 ].IntoStream( 0 );
			vertex[ 1 ].IntoStream( 1 );

			// 波テクスチャの描画
			wave->Begin( 0,	0 );
			// 描画開始
			wave->BeginPass( 0 );
			
			render.DrawPrimitive( Primitive::TRIANGLE_STRIP, 2 );

			wave->EndPass();
			wave->End();
			
			// 波の更新と描画終了

			
			// 法線マップの作成
			nomalRtTex->SetRenderTarget( 0 );
			SetRenderTargetFormat( 1 );

			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );
			
			// 描画に利用するテクスチャ
			nomal->SetTexture( "g_TextureWave", waveTex->GetTexture() );
			nomal->SetVector( "g_WaveTexOffset", waveTexOffset );

			nomal->SetTexture( "g_TextureHeight", hightTex[ 1 ].GetTexture() );
			nomal->SetVector( "g_TextureSize", waveTexSize );
			
			// 変数のアップデート
			nomal->CommitChanges();

			dec->Set();
			vertex[ 0 ].IntoStream( 0 );
			vertex[ 1 ].IntoStream( 1 );

			nomal->Begin( 0 ,0 );

			nomal->BeginPass( 0 );

			render.DrawPrimitive( Primitive::TRIANGLE_STRIP, 2 );
			

			nomal->EndPass();
			nomal->End();
			
			// 法線マップ作成終了

			render.SetZBufferState( true );
			backBuffer->SetRenderTarget( 0 );
			oldDepthTarget.SetDepthTarget();
			oldViewport.SetViewport();

			//// 海モデルの描画

			SetRenderTargetFormat( 1 );
			SetRenderTargetFormat( 2 );
			
			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );

			render.SetTexture( 0 , backTex->GetTexture() );
			back->Draw(math::Vector2( 0 , 0 ));
			

			//// 変換用の行列を渡す
			math::Matrix temp = wold * camera.GetView();
			sea->SetMatrix( "g_WorldToCameraMatrix", temp );
			sea->SetMatrix( "g_CameraToProjectionMatrix", camera.GetProjection() );
			sea->SetVector( "g_CameraPosition", math::Vector4(camera.eye.x,camera.eye.y,camera.eye.z,1.0f) );
			sea->SetVector( "g_DirectionLightVector", directionLightVector );
			sea->SetVector( "g_DirectionLightColor", directionLightColor );
			sea->SetVector( "g_SpecularLightColor", specularLightColor );

			sea->SetVector( "g_WaveTexOffset", waveTexOffset );

			// テクスチャを渡す
			sea->SetTexture( "g_TextureNormal", nomalTex->GetTexture() );
			sea->SetTexture( "g_TextureWave", waveTex->GetTexture() );
			sea->SetTexture( "g_TextureEnvMap", cubeTex->GetTexture() );

			sea->Begin( 0 , 0 );
			//render.SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME );
			for( int i = 0;i < (int)model->GetMaterialCount();i++ )
			{
				
				sea->CommitChanges();

				sea->BeginPass( 0 );

				model->Draw( i );

				sea->EndPass();
				
			}
			

			sea->End();
		
			heightMapIndex = 1 - heightMapIndex;
			
			
			//render.SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID );
			
			DrawFormatString( 710, 560, Color4( 255, 255, 0 ),  "SFPS[%.1f]" , FrameRateManage::GetStandardFrameRate() );
			DrawFormatString( 730, 580, Color4( 255, 255, 0 ),  "FPS[%.1f]" , FrameRateManage::GetFrameRate() );

			DrawFormatString( 680, 540, Color4( 255, 255, 0 ),  "LightVecY[%.1f]" , lightVec );
			
			// 描画終了
			SwapScreen();
		}

		// ウィンドウがノンアクティブの時の処理
		else
			system.Idle();
	}

	// リソースの解放

	sound.Release();
	SafeDelete( model );
	SafeDelete( nomal );
	SafeDelete( wave );
	SafeDelete( sea );
	SafeDelete( backBuffer );
	SafeDelete( ZBuffer );
	SafeDelete( waveTex );
	SafeArrayDelete( hightTex );
	SafeArrayDelete( hightRtTex );
	SafeArrayDelete( velocityTex );
	SafeArrayDelete( velocityRtTex );
	SafeDelete( nomalTex );
	SafeDelete( nomalRtTex );
	SafeDelete( cubeTex );
	SafeArrayDelete( vertex );
	SafeArrayDelete( pos );
	SafeArrayDelete( uv );
	SafeDelete( dec );
	SafeDelete( back );
	SafeDelete( backTex );


	SafeDelete( sprite );
	
	// ライブラリの終了
	system.Release();

	return 0;
}
//*
//	メモ
//	5月22日	海の作成
//			テクスチャーの読み込みとサーフェースの取得
//			を作成
//			次は波の更新
//	
//	6月29日	波と高さの更新と法線マップの動的テクスチャの
//			作成までは実装
//			次はそれらの動作確認と作成したテクスチャから
//			海を生成する
//	7月3日 テクスチャの自動生成と波の更新が完成
//	7月6日 スプライトから独自頂点フォーマットに変えて海完成
//
//
//*/