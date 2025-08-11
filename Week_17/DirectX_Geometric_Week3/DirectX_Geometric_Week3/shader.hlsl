cbuffer ConstantBuffer 
{
	matrix	worldViewProjection; 
	matrix  worldTransformation;
    float4  materialColour;
	float4	ambientLightColour;
	float4  directionalLightColour;
	float4  directionalLightVector;
};

struct VertexIn
{
	float3 InputPosition : POSITION;
	float3 Normal		 : NORMAL;
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

	// calculate the diffuse light and add it to the ambient light
	float4 vectorBackToLight = -directionalLightVector;
	float4 adjustedNormal = normalize(mul(worldTransformation, float4(vin.Normal, 0.0f)));
	float diffuseBrightness = saturate(dot(adjustedNormal, vectorBackToLight));
    vout.Colour = materialColour * saturate(ambientLightColour + diffuseBrightness * directionalLightColour);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Colour;
}


