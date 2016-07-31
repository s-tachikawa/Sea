
//==================================================================================================
// �ϐ�
//==================================================================================================
texture		g_TextureHeight;			// �e�N�X�`��
texture		g_TextureVelocity;			// �e�N�X�`��

float2		g_TextureOffsetTbl[8];		// �e�N�X�`���̃I�t�Z�b�g
float		g_WaveSpeed;				// ���x

//==================================================================================================
// �e�N�X�`���[�T���v���[
//==================================================================================================
sampler SamplerWave = sampler_state
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

//==================================================================================================
// ���_�t�H�[�}�b�g
//==================================================================================================

struct VS_TO_PS
{
	float4 Position		: POSITION;
	float2 Tex			: TEXCOORD;
};

struct PS_OUT
{
	float4 Height		: COLOR0;
	float4 Velocity		: COLOR1;
};

//==================================================================================================
// ���_�V�F�[�_�[
//==================================================================================================
VS_TO_PS VS_Sample( in float4 postion	: POSITION
					in float2 tex		: TEXCOORD)
{
	VS_TO_PS Out = (VS_TO_PS)0;

	// �ʒu���̂܂�
	Out.Position = postion;

	// �e�N�X�`�����W�����̂܂�
	Out.Tex = tex;

	return Out;
}

//==================================================================================================
// �s�N�Z���V�F�[�_�[
//==================================================================================================
PS_OUT PS_Sample( VS_TO_PS In )
{
	PS_OUT Out = (PS_OUT)0;

	float Height   = tex2D( SamplerWave, In.Tex );
	float Velocity = tex2D( SamplerVelocity, In.Tex );

	float4 HeightTbl = {
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[0] ).r,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[1] ).r,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[2] ).r,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[3] ).r,
	};
	float4 VelocityTbl = {
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[0] ).g,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[1] ).g,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[2] ).g,
		tex2D( SamplerWave, In.Tex + g_TextureOffsetTbl[3] ).g,
	};

	float4 H = HeightTbl - Height;
	float HForce = dot( H, float4( 1.0, 1.0, 1.0, 1.0 ) );

	float4 V = VelocityTbl - Velocity;
	float VForce = dot( V, float4( 1.0, 1.0, 1.0, 1.0 ) );
	
	Out.Velocity = VForce + (HForce * g_WaveSpeed);
	Out.Height = Height + Out.Velocity;

	return float4( Out.Height , Out.Velocity ,1.0f,1.0f );
}

//==================================================================================================
// �e�N�j�b�N
//==================================================================================================
technique WaveEffect
{
	pass p0
	{
		CULLMODE			= CCW;
		ZENABLE				= FALSE;
		ZWRITEENABLE		= FALSE;
		AlphaBlendEnable	= FALSE;
		
		VertexShader = compile vs_2_0 VS_Sample();
		PixelShader  = compile ps_2_0 PS_Sample();
	}
}
