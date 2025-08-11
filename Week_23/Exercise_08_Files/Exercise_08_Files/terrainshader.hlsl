cbuffer ConstantBuffer
{
    matrix completeTransformation;
    matrix worldTransformation;    
	float4 cameraPosition;
    float4 lightVector;			// the light's vector
    float4 lightColor;			// the light's color
    float4 ambientColor;		// the ambient light's color
    float4 diffuseCoefficient;	// The diffuse reflection cooefficient
	float4 specularCoefficient;	// The specular reflection cooefficient
	float  shininess;			// The shininess factor
	float  opacity;				// The opacity (transparency) of the material. 0 = fully transparent, 1 = fully opaque
	float2 padding;
}

Texture2D BlendMap : register(t0);
Texture2DArray TexturesArray : register(t1);

SamplerState ss
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexShaderInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float2 BlendMapTexCoord : TEXCOORD1;
};

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
    float4 VColor : COLOR;
	float4 PositionWS : TEXCOORD2;
	float4 NormalWS : TEXCOORD3;
	float2 TexCoord : TEXCOORD0;
	float2 BlendMapTexCoord : TEXCOORD1;
};

PixelShaderInput VS(VertexShaderInput vin)
{
    PixelShaderInput output;
	float3 position = vin.Position;
	output.Position = mul(completeTransformation, float4(position, 1.0f));
	output.PositionWS = mul(worldTransformation, float4(position, 1.0f));
	output.NormalWS = float4(mul((float3x3)worldTransformation, vin.Normal), 0.0f);
	output.TexCoord = vin.TexCoord;
	output.BlendMapTexCoord = vin.BlendMapTexCoord;
	
	// calculate the diffuse light and add it to the ambient light
    //float4 vectorBackToLight = normalize(lightVector); // directional light
    float4 vectorBackToLight = normalize(lightVector - output.PositionWS); // point light
    
    float4 adjustedNormal = normalize(output.NormalWS);
    float diffuseBrightness = saturate(dot(adjustedNormal, vectorBackToLight));
    
    output.VColor = saturate(ambientColor + diffuseCoefficient * diffuseBrightness * lightColor); // The vertex color (Gouraud)
	
	return output;
}

float4 PS(PixelShaderInput input) : SV_TARGET
{
	//per-vertex shading from Vertex shader (Gouraud)
    float4 shade = input.VColor;
	
	// Do your lighting calculations here - per-pixel shading (Phong)
	// calculate the diffuse light and add it to the ambient light
    
	//float4 vectorBackToLight = normalize(lightVector); // directional light
    float4 vectorBackToLight = normalize(lightVector - input.PositionWS); // point light
    
    float4 adjustedNormal = normalize(input.NormalWS);
    float diffuseBrightness = saturate(dot(adjustedNormal, vectorBackToLight));
	
    //shade = saturate(ambientColor); // Ambient
	//shade = saturate(diffuseBrightness * lightColor); // Diffuse
    shade = saturate(ambientColor + diffuseCoefficient * diffuseBrightness * lightColor); // ALL
    
	
	// The following code selects the appropriate pixel value from the 
	// texture array using the values from the blend map
	float4 color;

	// Sample layers in texture array.
	
	float4 c0 = TexturesArray.Sample(ss, float3(input.TexCoord, 0.0f));
	float4 c1 = TexturesArray.Sample(ss, float3(input.TexCoord, 1.0f));
	float4 c2 = TexturesArray.Sample(ss, float3(input.TexCoord, 2.0f));
	float4 c3 = TexturesArray.Sample(ss, float3(input.TexCoord, 3.0f));
	float4 c4 = TexturesArray.Sample(ss, float3(input.TexCoord, 4.0f));

	// Sample the blend map.
	float4 t = BlendMap.Sample(ss, input.BlendMapTexCoord);

	// Blend the layers on top of each other.
	color = c0;
	color = lerp(color, c1, t.r);
	color = lerp(color, c2, t.g);
	color = lerp(color, c3, t.b);
	color = lerp(color, c4, t.a);
    
	// Now adjust the colour using your calculated lighting values before returning
    //color = shade;
    color = shade * color;
    
	return color;
}