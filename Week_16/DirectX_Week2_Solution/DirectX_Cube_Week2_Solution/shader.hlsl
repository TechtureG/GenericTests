cbuffer ConstantBuffer 
{
	matrix worldViewProjection; 
};

struct VertexIn
{
	float4 InputPosition : POSITION;
	float4 Colour		 : COLOR;
};

struct VertexOut
{
	float4 OutputPosition : SV_POSITION;
	float4 Colour		  : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProjection, vin.InputPosition);
	// Copy the input colour to the output
	vout.Colour = vin.Colour;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Colour;
}


