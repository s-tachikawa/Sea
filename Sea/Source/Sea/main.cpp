
#include <iruna.h>

using namespace iruna;
using namespace	graphics;
using namespace math;
using namespace io;
using namespace core;

int WINAPI WinMain( HINSTANCE , HINSTANCE, LPTSTR, int )
{

	// ���C�u�����Ǘ��N���X
	// ���C�u�����̋N���A��ԍŏ��ɂ���
	System	system	= System::GetInstance();
	if( !system.Setup( 800, 600, true,  "�C������Ă݂�"  ) )	// WS_POPUP�Řg��������
		return -1;

	float				cameraAngle	= 0.f;
	float				cameraUp	= 4.0f;
	float				length		= D3DX_PI / cameraUp;
	// �J�����̏���
	Camera			camera;
	camera.eye		= math::Vector3( 0.f, 220.f, 250.f );
	camera.lookAt	= math::Vector3( 0.f, 0.f, -30.f );
	camera.upVector	= math::Vector3( 0.f, 1.f, 0.f );
	camera.fovY		= length;
	camera.aspect	= 800.f / 600.f;
	camera.zNear	= 0.1f;
	camera.zFar		= 500.f;



	// ���_�錾X�t�@�C���p
	static VertexDeclaration::Element VertexElement[] = {
		// ���_
		{ 0, 0, D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
		// uv���W
		{ 1, 0, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	
	};

	math::Matrix wold;
	D3DXMatrixTranslation( &wold , 0.0f , 0.0f , 0.0f );
	DeviceController render;

	// ���f���̃��[�h
	Model* model = new Model;
	model->Load( "../Resource/model/Sea.x" );
	model->RemakeVertexFormat( VertexElement );

	// �G�t�F�N�g�̃��[�h
	Effect* nomal	= new Effect;
	Effect* wave		= new Effect;
	Effect* sea		= new Effect;
	nomal->Create( "../Resource/efect/CreateNormal.fx",1 );
	wave->Create( "../Resource/efect/UpdateHeightVelocity.fx",1  );
	sea->Create(  "../Resource/efect/Sea.fx",1  );

	
	// �o�b�N�o�b�t�@�T�[�t�F�[�X
	Surface* backBuffer			= new Surface;
	// Z�o�b�t�@
	Surface* ZBuffer			= new Surface;
	

	// �g�̊�e�N�X�`���[
	Texture* waveTex			= new Texture;
	waveTex->Load( "../Resource/texture/Wave.dds" );

	// �����}�b�v�̓ǂݍ���
	Texture* hightTex			= new Texture[ 2 ];
	hightTex[ 0 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );
	hightTex[ 1 ].Load( "../Resource/texture/Height.dds" );

	// �����}�b�v�̃T�[�t�F�[�X�擾
	Surface* hightRtTex			= new Surface[ 2 ];
	hightTex[ 0 ].GetSurface( hightRtTex[ 0 ] );
	hightTex[ 1 ].GetSurface( hightRtTex[ 1 ] );

	// �g�̑����}�b�v�̓ǂݍ���
	Texture* velocityTex		= new Texture[ 2 ];
	velocityTex[ 0 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );
	velocityTex[ 1 ].Create( 256,256 , D3DUSAGE_RENDERTARGET );

	// �����}�b�v�̃T�[�t�F�[�X�擾
	Surface* velocityRtTex		= new Surface[ 2 ];
	velocityTex[ 0 ].GetSurface( velocityRtTex[ 0 ] );
	velocityTex[ 1 ].GetSurface( velocityRtTex[ 1 ] );

	// �@��}�b�v
	Texture* nomalTex			= new Texture;
	nomalTex->Create( 256,256 , D3DUSAGE_RENDERTARGET );

	// �@��}�b�v�̃T�[�t�F�[�X�̎擾
	Surface* nomalRtTex			= new Surface;
	nomalTex->GetSurface( (*nomalRtTex) );


	// ���}�b�v�̓ǂݍ���
	CubeTexture* cubeTex = new CubeTexture;
	cubeTex->Load( "../Resource/texture/Environment.dds" );



	// �e�N�X�`���[�̃I�t�Z�b�g(�e�N�X�`���[�̏㉺���E���擾���邽�߂̂���)
	math::Vector4 waveTexOffset = math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
	math::Vector4 waveTexSize = math::Vector4( 256.0f, 256.0f, 0.0f, 0.0f );
	// �g�e�N�X�`���[�̃C���f�b�N�X
	int heightMapIndex = 1;

	// ���C�g�̌v�Z
	float lightVec = 0.01f;
	// ���s�����p���C�g�x�N�g��
	math::Vector4 directionLightVector	= math::Vector4( 0.5f, lightVec, 0.5f, 0.0f );

	// ���s�����p���C�g�F
	math::Vector4 directionLightColor	= math::Vector4( 0.4f, 0.4f, 0.6f, 0.0f );
	// �X�y�L�����[�F
	math::Vector4 specularLightColor	= math::Vector4( 1.2f, 0.6f, 0.2f, 0.0f );


	// ���_�o�b�t�@,���_�錾�쐬
	VertexBuffer* vertex = new VertexBuffer[ 2 ];
	math::Vector4* pos = new math::Vector4[ 4 ];
	math::Vector2* uv = new math::Vector2[ 4 ];

	VertexDeclaration* dec = new VertexDeclaration;

	static VertexDeclaration::Element Vertex2DElement[] = {
		// ���_
		{ 0, 0, D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
		// uv���W
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


	// �X�v���C�g
	Sprite* sprite = new Sprite;
	sprite->SetSize( math::Vector2( 256,256 ) );

	// �w�i
	Sprite* back = new Sprite;
	back->SetSize( math::Vector2( 800,600 ) );
	Texture* backTex = new Texture;
	backTex->Load("../Resource/texture/14656967_m.jpg"  );
	back->SetColor( Color4( 255,255,255,255 ) );
	
	sound::StreamSound sound;
	sound.LoadSound( "../Resource/sound/sea.ogg" );
	sound.PlaySound();
	sound.SetVolume( 0.5f );
	// ���C�����[�v
	while( system.Run() )
	{
	
		// �E�B���h�E���A�N�e�B�u�̎��������s
		if( system.IsActive() )
		{

			//	�L�[�̏���(���߂�̍X�V)
			if( io::CheckKeyState( DIK_LEFT ) )
				cameraAngle += 0.01f;
			if( io::CheckKeyState( DIK_RIGHT ) )
				cameraAngle -= 0.01f;
			
			// ���C�g�̍X�V
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
			
			// �g�̍X�V�ƕ`��

			//	�g�e�N�X�`����UV�I�t�Z�b�g�l�̍X�V
			waveTexOffset.x += 0.0010f;
			waveTexOffset.y -= 0.0015f;
			// UV���W�Ȃ̂�1.0�𒴂��Ȃ��悤�ɂ���
			while ( waveTexOffset.x < 0.0f ) waveTexOffset.x += 1.0f;
			while ( waveTexOffset.y < 0.0f ) waveTexOffset.y += 1.0f;


			ClearScreen();
			// �o�b�N�o�b�t�@���Ƃ��Ă���
			GetBackBuffer( *backBuffer );
			Viewport	oldViewport;
			Viewport	newViewport;
			GetViewport( oldViewport );
			newViewport.SetSize( 0, 0, static_cast< int >( 256 ), static_cast< int >( 256 ) );

			// �g�}�b�v�ƍ����}�b�v�̍X�V
			velocityRtTex[ heightMapIndex ].SetRenderTarget( 1 );
			hightRtTex[ heightMapIndex ].SetRenderTarget( 0 );
			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);
			

			// Z�o�b�t�@�͎g��Ȃ�
			render.SetZBufferState( false );
			Surface		oldDepthTarget;
			Surface		newDepthTarget;
			GetDepthTarget( oldDepthTarget );
			newDepthTarget.SetDepthTarget();	//��T�[�t�F�X�Ŗ�����newViewport.SetViewport();


			// �e�N�X�`���̃I�t�Z�b�g�e�[�u��
			math::Vector4 TextureOffsetTbl[] = {
				math::Vector4(-1.0f,  0.0f, 0.0f, 0.0f),
				math::Vector4(+1.0f,  0.0f, 0.0f, 0.0f),
				math::Vector4( 0.0f, -1.0f, 0.0f, 0.0f),
				math::Vector4( 0.0f, +1.0f, 0.0f, 0.0f),
			};
			// �e�N�X�`���[�̃T�C�Y�ɍ��킹��
			for ( int i = 0; i < 4; i++ )
			{
				TextureOffsetTbl[i].x /= waveTexSize.x;
				TextureOffsetTbl[i].y /= waveTexSize.y;
			}
			wave->SetTexture( "g_TextureHeight", hightTex[ 1 - heightMapIndex ].GetTexture() );
			wave->SetTexture( "g_TextureVelocity", velocityTex[ 1 - heightMapIndex ].GetTexture() );
			wave->SetVectorArayy( "g_TextureOffsetTbl", TextureOffsetTbl, 4 );
			wave->SetFloat( "g_WaveSpeed", 0.01f );

			// �l���ύX�������Ƃ��f�o�C�X�ɓ`����
			wave->CommitChanges();

			dec->Set();
			vertex[ 0 ].IntoStream( 0 );
			vertex[ 1 ].IntoStream( 1 );

			// �g�e�N�X�`���̕`��
			wave->Begin( 0,	0 );
			// �`��J�n
			wave->BeginPass( 0 );
			
			render.DrawPrimitive( Primitive::TRIANGLE_STRIP, 2 );

			wave->EndPass();
			wave->End();
			
			// �g�̍X�V�ƕ`��I��

			
			// �@���}�b�v�̍쐬
			nomalRtTex->SetRenderTarget( 0 );
			SetRenderTargetFormat( 1 );

			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );
			
			// �`��ɗ��p����e�N�X�`��
			nomal->SetTexture( "g_TextureWave", waveTex->GetTexture() );
			nomal->SetVector( "g_WaveTexOffset", waveTexOffset );

			nomal->SetTexture( "g_TextureHeight", hightTex[ 1 ].GetTexture() );
			nomal->SetVector( "g_TextureSize", waveTexSize );
			
			// �ϐ��̃A�b�v�f�[�g
			nomal->CommitChanges();

			dec->Set();
			vertex[ 0 ].IntoStream( 0 );
			vertex[ 1 ].IntoStream( 1 );

			nomal->Begin( 0 ,0 );

			nomal->BeginPass( 0 );

			render.DrawPrimitive( Primitive::TRIANGLE_STRIP, 2 );
			

			nomal->EndPass();
			nomal->End();
			
			// �@���}�b�v�쐬�I��

			render.SetZBufferState( true );
			backBuffer->SetRenderTarget( 0 );
			oldDepthTarget.SetDepthTarget();
			oldViewport.SetViewport();

			//// �C���f���̕`��

			SetRenderTargetFormat( 1 );
			SetRenderTargetFormat( 2 );
			
			ClearBackBuffer( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );

			render.SetTexture( 0 , backTex->GetTexture() );
			back->Draw(math::Vector2( 0 , 0 ));
			

			//// �ϊ��p�̍s���n��
			math::Matrix temp = wold * camera.GetView();
			sea->SetMatrix( "g_WorldToCameraMatrix", temp );
			sea->SetMatrix( "g_CameraToProjectionMatrix", camera.GetProjection() );
			sea->SetVector( "g_CameraPosition", math::Vector4(camera.eye.x,camera.eye.y,camera.eye.z,1.0f) );
			sea->SetVector( "g_DirectionLightVector", directionLightVector );
			sea->SetVector( "g_DirectionLightColor", directionLightColor );
			sea->SetVector( "g_SpecularLightColor", specularLightColor );

			sea->SetVector( "g_WaveTexOffset", waveTexOffset );

			// �e�N�X�`����n��
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
			
			// �`��I��
			SwapScreen();
		}

		// �E�B���h�E���m���A�N�e�B�u�̎��̏���
		else
			system.Idle();
	}

	// ���\�[�X�̉��

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
	
	// ���C�u�����̏I��
	system.Release();

	return 0;
}
//*
//	����
//	5��22��	�C�̍쐬
//			�e�N�X�`���[�̓ǂݍ��݂ƃT�[�t�F�[�X�̎擾
//			���쐬
//			���͔g�̍X�V
//	
//	6��29��	�g�ƍ����̍X�V�Ɩ@���}�b�v�̓��I�e�N�X�`����
//			�쐬�܂ł͎���
//			���͂����̓���m�F�ƍ쐬�����e�N�X�`������
//			�C�𐶐�����
//	7��3�� �e�N�X�`���̎��������Ɣg�̍X�V������
//	7��6�� �X�v���C�g����Ǝ����_�t�H�[�}�b�g�ɕς��ĊC����
//
//
//*/