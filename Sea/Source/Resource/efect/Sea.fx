
//==================================================================================================
// 変数
//==================================================================================================
float4x4	g_WorldToCameraMatrix;			// ワールド座標⇒カメラ座標変換
float4x4	g_CameraToProjectionMatrix;		// カメラ座標⇒プロジェクション座標変換

float4		g_CameraPosition;				// カメラ位置
float4		g_DirectionLightVector;			// ライト方向
float4		g_DirectionLightColor;			// ライト色
float4		g_SpecularLightColor;			// スペキュラー色

float4		g_WaveTexOffset;				// 波テクスチャオフセット

texture		g_TextureNormal;				// 法線テクスチャ
texture		g_TextureWave;					// 高さテクスチャ
textureCUBE g_TextureEnvMap;				// 環境マップテクスチャ
//==================================================================================================
// テクスチャーサンプラー
//==================================================================================================
sampler SamplerNormal = sampler_state
{
	// 利用するテクスチャ
	Texture = <g_TextureNormal>;

	// テクスチャのフィルタリング方法
	MinFilter = Anisotropic;	// 異方性サンプリング
	MagFilter = Anisotropic;	// 異方性サンプリング
	MipFilter = Point;			// 最近点サンプリング

	// テクスチャの範囲外参照時の挙動
	AddressU = Wrap;		// 繰り返し
	AddressV = Wrap;		// 繰り返し

	// 異方性の値
	MaxAnisotropy = 8; 
};

sampler SamplerWave = sampler_state
{
	// 利用するテクスチャ
	Texture = <g_TextureWave>;

	// テクスチャのフィルタリング方法
	MinFilter = Point;		// 最近点サンプリング
	MagFilter = Point;		// 最近点サンプリング
	MipFilter = Point;		// 最近点サンプリング

	// テクスチャの範囲外参照時の挙動
	AddressU = Wrap;		// 繰り返し
	AddressV = Wrap;		// 繰り返し
};



samplerCUBE cubeTexSampler = sampler_state
{                    
   Texture = <g_TextureEnvMap>;           
    MinFilter = LINEAR;            
    MagFilter = LINEAR;            
    MipFilter = LINEAR;            
};                                 


//==================================================================================================
// 頂点フォーマット
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
// 頂点シェーダー
//==================================================================================================
VS_TO_PS VS_Sample( CPU_TO_VS In )
{
	VS_TO_PS Out;

	// 高さテクスチャをサンプリング
	float Height = tex2Dlod( SamplerWave, In.Tex + g_WaveTexOffset ).r;

	// テクスチャ座標はそのまま渡す
	Out.Tex = In.Tex.xy;

	// 高さを考慮してワールドの位置を作成
	float4 WorldPosition = In.Position + float4( 0.0, Height * 3.0f, 0.0, 0.0 );

	// ワールド空間からからカメラ空間へ変換する
	float4 CameraPosition = mul( WorldPosition, g_WorldToCameraMatrix );

	// カメラ空間からプロジェクション座標に変換して渡す
	Out.Position = mul( CameraPosition, g_CameraToProjectionMatrix );

	// ワールドの位置
	Out.WorldPosition = WorldPosition;

	return Out;
}

//==================================================================================================
// ピクセルシェーダー
//==================================================================================================
float4 PS_Sample( VS_TO_PS In ) : COLOR
{
	float4 Color = { 0.0, 0.0, 0.0, 1.0 };

	// 法線マップの値をとってくる
	float3 Normal = normalize( 2.0f * tex2D( SamplerNormal, In.Tex ) - 0.5f );

	// 視線ベクトル＆反射ベクトル
	float3 EyeVec = normalize( In.WorldPosition - g_CameraPosition );
	float3 ReflectVec = reflect( EyeVec, Normal );

	// 適当フレネル項
	float fFresnel = pow( 1.0 - dot( -EyeVec, Normal ), 2.0 );

	// 適当半球ライト
	Color.rgb = lerp( float3( 0.05, 0.05, 0.08 ), float3( 0.10, 0.07, 0.05 ), (Normal.y * 0.5) + 0.5 );

	// 適当平行光源ライト
	float fLightPow = (dot( Normal, g_DirectionLightVector ) * 0.5) + 0.5;
	float4 LightColor = g_DirectionLightColor * fLightPow;

	// 環境マップ

	float3 TexReflect = texCUBE( cubeTexSampler,ReflectVec).rgb;
	float3 TexRefract = texCUBE( cubeTexSampler, refract( EyeVec, Normal, 0.95 ) ).rgb;
	Color.rgb = lerp( TexRefract, LightColor, fFresnel ) * 0.5;
	Color.rgb += TexReflect * fFresnel * 0.3;

	// 適当スペキュラー
	float fSpecular = pow( max( 0.0, dot( -ReflectVec, g_DirectionLightVector ) ), 8.0 );
	Color.rgb += g_SpecularLightColor.rgb * fSpecular * fFresnel;

	// ライティング結果を出力
	return Color;
}

//==================================================================================================
// テクニック
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
