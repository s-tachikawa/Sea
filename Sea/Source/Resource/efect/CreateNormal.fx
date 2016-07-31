
//==================================================================================================
// 変数
//==================================================================================================
texture		g_TextureHeight;			// テクスチャ
texture		g_TextureWave;				// 高さテクスチャ
float4		g_TextureSize;				// テクスチャのサイズ
float4		g_WaveTexOffset;			// 波テクスチャオフセット

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

sampler SamplerWave = sampler_state
{
	// 利用するテクスチャ
	Texture = <g_TextureWave>;

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
	float2 Tex[4]		: TEXCOORD;
};

//==================================================================================================
// 頂点シェーダー
//==================================================================================================
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// 位置そのまま
	
	Out.Position = In.Position;

	// テクスチャ座標は中心からの４点
	float2 fInvSize = float2( 1.0, 1.0 ) / g_TextureSize.xy;

	Out.Tex[0] = In.Tex + float2( 0.0, -fInvSize.y );		// 上
	Out.Tex[1] = In.Tex + float2( 0.0, +fInvSize.y );		// 下
	Out.Tex[2] = In.Tex + float2( -fInvSize.x, 0.0 );		// 左
	Out.Tex[3] = In.Tex + float2( +fInvSize.x, 0.0 );		// 右

	return Out;
}

//==================================================================================================
// ピクセルシェーダー
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
