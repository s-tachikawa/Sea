
//==================================================================================================
// 変数
//==================================================================================================
texture		g_TextureHeight;			// テクスチャ
texture		g_TextureVelocity;			// テクスチャ

float2		g_TextureOffsetTbl[8];		// テクスチャのオフセット
float		g_WaveSpeed;				// 速度

//==================================================================================================
// テクスチャーサンプラー
//==================================================================================================
sampler SamplerHeight = sampler_state
{
	// 利用するテクスチャ
	Texture = <g_TextureHeight>;

	// テクスチャのフィルタリング方法
	MinFilter = Point;		// 最近点平均サンプリング
	MagFilter = Point;		// 最近平均サンプリング
	MipFilter = Point;		// 最近点サンプリング

	// テクスチャの範囲外参照時の挙動
	AddressU = Wrap;		// 繰り返さない
	AddressV = Wrap;		// 繰り返さない
};

sampler SamplerVelocity = sampler_state
{
	// 利用するテクスチャ
	Texture = <g_TextureVelocity>;

	// テクスチャのフィルタリング方法
	MinFilter = Point;		// 最近点平均サンプリング
	MagFilter = Point;		// 最近平均サンプリング
	MipFilter = Point;		// 最近点サンプリング

	// テクスチャの範囲外参照時の挙動
	AddressU = Wrap;		// 繰り返さない
	AddressV = Wrap;		// 繰り返さない
};

//==================================================================================================
// 頂点フォーマット
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
// 頂点シェーダー
//==================================================================================================
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// 位置そのまま
	Out.Position = In.Position;

	// テクスチャ座標もそのまま
	Out.Tex = In.Tex;

	return Out;
}

//==================================================================================================
// ピクセルシェーダー
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
// テクニック
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
