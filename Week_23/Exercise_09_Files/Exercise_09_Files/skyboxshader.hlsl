
cbuffer ConstantBuffer 
{
    matrix worldViewProjection;
    matrix worldTransformation;
	float4 ambientColour;   // The ambient light's colour
};

Texture2D Texture;
SamplerState ss;

struct VertexIn
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 Position  : SV_POSITION;
    float4 Colour	 : COLOUR;
	float2 TexCoord	 : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.Position = mul(worldViewProjection, float4(vin.Position, 1.0f));
	
	// use the ambient light
	vout.Colour = saturate(ambientColour);

	vout.TexCoord = vin.TexCoord;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Colour * Texture.Sample(ss, pin.TexCoord);
}


