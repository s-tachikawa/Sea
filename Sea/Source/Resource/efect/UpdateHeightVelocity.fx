
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

sampler SamplerVelocity = sampler_state
{
	// ���p����e�N�X�`��
	Texture = <g_TextureVelocity>;

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
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// �ʒu���̂܂�
	Out.Position = In.Position;

	// �e�N�X�`�����W�����̂܂�
	Out.Tex = In.Tex;

	return Out;
}

//==================================================================================================
// �s�N�Z���V�F�[�_�[
//==================================================================================================
PS_OUT PS_Sample( VS_TO_PS In )
{
	PS_OUT Out;

	float Height   = tex2D( SamplerHeight, In.Tex );
	float Velocity = tex2D( SamplerVelocity, In.Tex );

	float4 HeightTbl = {
		tex2D( SamplerHeight, In.Tex + g_TextureOffsetTbl[0] ).r,
		tex2D( SamplerHeight, In.Tex + g_TextureOffsetTbl[1] ).r,
		tex2D( SamplerHeight, In.Tex + g_TextureOffsetTbl[2] ).r,
		tex2D( SamplerHeight, In.Tex + g_TextureOffsetTbl[3] ).r,
	};

	float4 fForceTbl = HeightTbl - Height;
	float fForce = dot( fForceTbl, float4( 1.0, 1.0, 1.0, 1.0 ) );

	Out.Velocity = Velocity - (fForce + g_WaveSpeed);
	//Out.Velocity.a =  1.0f;
	//Out.Velocity.r = 1.0f;
	//Out.Velocity.g = 0.0f;
	//Out.Velocity.b = 0.0f;
	
	Out.Height = Height + Out.Velocity;
	//Out.Height.a = 1.0f;
	//Out.Height.r = 1.0f;
	//Out.Height.g = 1.0f;
	//Out.Height.b = 1.0f;
	
	return Out;
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
