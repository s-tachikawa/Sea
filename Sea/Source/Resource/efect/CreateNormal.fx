
//==================================================================================================
// �ϐ�
//==================================================================================================
texture		g_TextureHeight;			// �e�N�X�`��
texture		g_TextureWave;				// �����e�N�X�`��
float4		g_TextureSize;				// �e�N�X�`���̃T�C�Y
float4		g_WaveTexOffset;			// �g�e�N�X�`���I�t�Z�b�g

//==================================================================================================
// �e�N�X�`���[�T���v���[
//==================================================================================================
sampler SamplerHeight = sampler_state
{
	// ���p����e�N�X�`��
	Texture = <g_TextureHeight>;

	// �e�N�X�`���̃t�B���^�����O���@
	MinFilter = Point;		// �ŋߓ_���σT���v�����O
	MagFilter = Point;		// �ŋߕ��σT���v�����O
	MipFilter = Point;		// �ŋߓ_�T���v�����O

	// �e�N�X�`���͈̔͊O�Q�Ǝ��̋���
	AddressU = Wrap;		// �J��Ԃ��Ȃ�
	AddressV = Wrap;		// �J��Ԃ��Ȃ�
};

sampler SamplerWave = sampler_state
{
	// ���p����e�N�X�`��
	Texture = <g_TextureWave>;

	// �e�N�X�`���̃t�B���^�����O���@
	MinFilter = Point;		// �ŋߓ_���σT���v�����O
	MagFilter = Point;		// �ŋߕ��σT���v�����O
	MipFilter = Point;		// �ŋߓ_�T���v�����O

	// �e�N�X�`���͈̔͊O�Q�Ǝ��̋���
	AddressU = Wrap;		// �J��Ԃ��Ȃ�
	AddressV = Wrap;		// �J��Ԃ��Ȃ�
};

//==================================================================================================
// ���_�t�H�[�}�b�g
//==================================================================================================
struct CPU_TO_VS
{
	float4 Position		: POSITION;
	float4 col			: COLOR0;
	float2 Tex			: TEXCOORD;
};

struct VS_TO_PS
{
	float4 Position		: POSITION;
	float2 Tex[4]		: TEXCOORD;
};

//==================================================================================================
// ���_�V�F�[�_�[
//==================================================================================================
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// �ʒu���̂܂�
	
	Out.Position = In.Position;

	// �e�N�X�`�����W�͒��S����̂S�_
	float2 fInvSize = float2( 1.0, 1.0 ) / g_TextureSize.xy;

	Out.Tex[0] = In.Tex + float2( 0.0, -fInvSize.y );		// ��
	Out.Tex[1] = In.Tex + float2( 0.0, +fInvSize.y );		// ��
	Out.Tex[2] = In.Tex + float2( -fInvSize.x, 0.0 );		// ��
	Out.Tex[3] = In.Tex + float2( +fInvSize.x, 0.0 );		// �E

	return Out;
}

//==================================================================================================
// �s�N�Z���V�F�[�_�[
//==================================================================================================
float4 PS_Sample( VS_TO_PS In ) : COLOR0
{
	float WaveU = tex2D( SamplerWave, In.Tex[0] + g_WaveTexOffset );
	float WaveD = tex2D( SamplerWave, In.Tex[1] + g_WaveTexOffset );
	float WaveL = tex2D( SamplerWave, In.Tex[2] + g_WaveTexOffset );
	float WaveR = tex2D( SamplerWave, In.Tex[3] + g_WaveTexOffset );

	float WaveHx = (WaveR - WaveL) * 10.0f;
	float WaveHy = (WaveU - WaveD) * 10.0f;

	float HeightU = tex2D( SamplerHeight, In.Tex[0] );
	float HeightD = tex2D( SamplerHeight, In.Tex[1] );
	float HeightL = tex2D( SamplerHeight, In.Tex[2] );
	float HeightR = tex2D( SamplerHeight, In.Tex[3] );

	float HeightHx = (HeightR - HeightL) * 3.0f;
	float HeightHy = (HeightU - HeightD) * 3.0f;

	float3 AxisU = { 1.0, HeightHx + WaveHx, 0.0f };
	float3 AxisV = { 0.0, HeightHy + WaveHy, 1.0f };

	float3 Out = (normalize( cross( AxisU, AxisV ) ) * 0.5) + 0.5;

	return float4( Out.x,Out.y,Out.z, 1.0f );
}

//==================================================================================================
// �e�N�j�b�N
//==================================================================================================
technique Technique_Sample
{
	pass
	{
		CULLMODE			= CCW;
		ZENABLE				= FALSE;
		ZWRITEENABLE		= FALSE;
		AlphaBlendEnable	= FALSE;
		VertexShader = compile vs_3_0 VS_Sample();
		PixelShader  = compile ps_3_0 PS_Sample();
	}
}
