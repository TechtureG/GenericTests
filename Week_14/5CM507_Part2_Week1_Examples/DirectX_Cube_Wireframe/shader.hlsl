
cbuffer ConstantBuffer 
{
	matrix worldViewProjection; 
};

struct VertexIn
{
	float4 InputPosition : POSITION;
};

struct VertexOut
{
	float4 OutputPosition : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProjection, vin.InputPosition);
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


