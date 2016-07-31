
//==================================================================================================
// �ϐ�
//==================================================================================================
float4x4	g_WorldToCameraMatrix;			// ���[���h���W�˃J�������W�ϊ�
float4x4	g_CameraToProjectionMatrix;		// �J�������W�˃v���W�F�N�V�������W�ϊ�

float4		g_CameraPosition;				// �J�����ʒu
float4		g_DirectionLightVector;			// ���C�g����
float4		g_DirectionLightColor;			// ���C�g�F
float4		g_SpecularLightColor;			// �X�y�L�����[�F

float4		g_WaveTexOffset;				// �g�e�N�X�`���I�t�Z�b�g

texture		g_TextureNormal;				// �@���e�N�X�`��
texture		g_TextureWave;					// �����e�N�X�`��
textureCUBE g_TextureEnvMap;				// ���}�b�v�e�N�X�`��
//==================================================================================================
// �e�N�X�`���[�T���v���[
//==================================================================================================
sampler SamplerNormal = sampler_state
{
	// ���p����e�N�X�`��
	Texture = <g_TextureNormal>;

	// �e�N�X�`���̃t�B���^�����O���@
	MinFilter = Anisotropic;	// �ٕ����T���v�����O
	MagFilter = Anisotropic;	// �ٕ����T���v�����O
	MipFilter = Point;			// �ŋߓ_�T���v�����O

	// �e�N�X�`���͈̔͊O�Q�Ǝ��̋���
	AddressU = Wrap;		// �J��Ԃ�
	AddressV = Wrap;		// �J��Ԃ�

	// �ٕ����̒l
	MaxAnisotropy = 8; 
};

sampler SamplerWave = sampler_state
{
	// ���p����e�N�X�`��
	Texture = <g_TextureWave>;

	// �e�N�X�`���̃t�B���^�����O���@
	MinFilter = Point;		// �ŋߓ_�T���v�����O
	MagFilter = Point;		// �ŋߓ_�T���v�����O
	MipFilter = Point;		// �ŋߓ_�T���v�����O

	// �e�N�X�`���͈̔͊O�Q�Ǝ��̋���
	AddressU = Wrap;		// �J��Ԃ�
	AddressV = Wrap;		// �J��Ԃ�
};



samplerCUBE cubeTexSampler = sampler_state
{                    
   Texture = <g_TextureEnvMap>;           
    MinFilter = LINEAR;            
    MagFilter = LINEAR;            
    MipFilter = LINEAR;            
};                                 


//==================================================================================================
// ���_�t�H�[�}�b�g
//==================================================================================================
struct CPU_TO_VS
{
	float4 Position		: POSITION;
	float4 Tex			: TEXCOORD0;
};

struct VS_TO_PS
{
	float4 Position			: POSITION;
	float2 Tex				: TEXCOORD0;
	float3 WorldPosition	: TEXCOORD1;
};

//==================================================================================================
// ���_�V�F�[�_�[
//==================================================================================================
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// �����e�N�X�`�����T���v�����O
	float Height = tex2Dlod( SamplerWave, In.Tex + g_WaveTexOffset ).r;

	// �e�N�X�`�����W�͂��̂܂ܓn��
	Out.Tex = In.Tex.xy;

	// �������l�����ă��[���h�̈ʒu���쐬
	float4 WorldPosition = In.Position + float4( 0.0, Height * 3.0f, 0.0, 0.0 );

	// ���[���h��Ԃ��炩��J������Ԃ֕ϊ�����
	float4 CameraPosition = mul( WorldPosition, g_WorldToCameraMatrix );

	// �J������Ԃ���v���W�F�N�V�������W�ɕϊ����ēn��
	Out.Position = mul( CameraPosition, g_CameraToProjectionMatrix );

	// ���[���h�̈ʒu
	Out.WorldPosition = WorldPosition;

	return Out;
}

//==================================================================================================
// �s�N�Z���V�F�[�_�[
//==================================================================================================
float4 PS_Sample( VS_TO_PS In ) : COLOR
{
	float4 Color = { 0.0, 0.0, 0.0, 1.0 };

	// �@���}�b�v�̒l���Ƃ��Ă���
	float3 Normal = normalize( 2.0f * tex2D( SamplerNormal, In.Tex ) - 0.5f );

	// �����x�N�g�������˃x�N�g��
	float3 EyeVec = normalize( In.WorldPosition - g_CameraPosition );
	float3 ReflectVec = reflect( EyeVec, Normal );

	// �K���t���l����
	float fFresnel = pow( 1.0 - dot( -EyeVec, Normal ), 2.0 );

	// �K���������C�g
	Color.rgb = lerp( float3( 0.05, 0.05, 0.08 ), float3( 0.10, 0.07, 0.05 ), (Normal.y * 0.5) + 0.5 );

	// �K�����s�������C�g
	float fLightPow = (dot( Normal, g_DirectionLightVector ) * 0.5) + 0.5;
	float4 LightColor = g_DirectionLightColor * fLightPow;

	// ���}�b�v

	float3 TexReflect = texCUBE( cubeTexSampler,ReflectVec).rgb;
	float3 TexRefract = texCUBE( cubeTexSampler, refract( EyeVec, Normal, 0.95 ) ).rgb;
	Color.rgb = lerp( TexRefract, LightColor, fFresnel ) * 0.5;
	Color.rgb += TexReflect * fFresnel * 0.3;

	// �K���X�y�L�����[
	float fSpecular = pow( max( 0.0, dot( -ReflectVec, g_DirectionLightVector ) ), 8.0 );
	Color.rgb += g_SpecularLightColor.rgb * fSpecular * fFresnel;

	// ���C�e�B���O���ʂ��o��
	return Color;
}

//==================================================================================================
// �e�N�j�b�N
//==================================================================================================
technique Technique_Sample
{
	pass
	{
		CULLMODE			= CCW;
		ZENABLE				= TRUE;
		ZWRITEENABLE		= TRUE;
		AlphaBlendEnable	= TRUE;
		
		VertexShader = compile vs_3_0 VS_Sample();
		PixelShader  = compile ps_3_0 PS_Sample();
	}
}
