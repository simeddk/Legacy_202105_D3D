//-----------------------------------------------------------------------------
//TextureMap
//-----------------------------------------------------------------------------
Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

//-----------------------------------------------------------------------------
//GlobalLight
//-----------------------------------------------------------------------------
struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

//-----------------------------------------------------------------------------
//Material
//-----------------------------------------------------------------------------
struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
};

//-----------------------------------------------------------------------------
//Functions
//-----------------------------------------------------------------------------
void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);

    [flatten]
    if (any(sampling.rgb))
        color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

float3 MaterialToColor(MaterialDesc result)
{
    return float4(result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

//-----------------------------------------------------------------------------
//Lighting
//-----------------------------------------------------------------------------
void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;

    //Ambient
    output.Ambient = GlobalLight.Ambient * Material.Ambient;

    
    float3 direction = -GlobalLight.Direction;
    float lambert = dot(normalize(normal), direction);

    float3 E = normalize(ViewPosition() - wPosition);

    [flatten]
    if (lambert > 0.0f)
    {
        //Diffuse
        output.Diffuse = Material.Diffuse * lambert; 

        //Specular
        [flatten]
        if (Material.Specular.a > 0.0f)
        {
            float3 R = normalize(reflect(GlobalLight.Direction, normal));
            float RdotE = saturate(dot(R, E));

            float specular = pow(RdotE, Material.Specular.a);
            output.Specular = Material.Specular * specular * GlobalLight.Specular; 
        }
    }

    //Emissive
    [flatten]
    if (Material.Emissive.a > 0.0f)
    {
        float NdotE = dot(normalize(normal), E);

        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - NdotE);

        output.Emissive = Material.Emissive * emissive;
    }
}

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float3 map = NormalMap.Sample(samp, uv).rgb;

    [flatten]
    if (any(map.rgb) == false)
        return;

    float3 pxDirection = map.rgb * 2.0f - 1.0f; //px color -> direction

    float3 N = normalize(normal);
    float3 T = normalize(tangent - max(dot(tangent, N) * N, 0.0001f));
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    pxDirection = mul(pxDirection, TBN);

    Material.Diffuse *= saturate(dot(pxDirection, -GlobalLight.Direction));
}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);
}