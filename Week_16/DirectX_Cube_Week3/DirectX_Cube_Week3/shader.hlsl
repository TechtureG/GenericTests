cbuffer ConstantBuffer 
{
	matrix	worldViewProjection; 
    float4  materialColour;
	float4	ambientLightColour;
};

struct VertexIn
{
	float3 InputPosition : POSITION;
};

struct VertexOut
{
	float4 OutputPosition	: SV_POSITION;
	float4 Colour			: COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProjection, float4(vin.InputPosition, 1.0f));
    vout.Colour = saturate(materialColour * ambientLightColour);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Colour;
}


