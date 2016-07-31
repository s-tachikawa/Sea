
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
sampler SamplerWave = sampler_state
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

//==================================================================================================
// 頂点フォーマット
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
// 頂点シェーダー
//==================================================================================================
VS_TO_PS VS_Sample( in float4 postion	: POSITION
					in float2 tex		: TEXCOORD)
{
	VS_TO_PS Out = (VS_TO_PS)0;

	// 位置そのまま
	Out.Position = postion;

	// テクスチャ座標もそのまま
	Out.Tex = tex;

	return Out;
}

//==================================================================================================
// ピクセルシェーダー
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
// テクニック
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
