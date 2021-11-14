//-----------------------------------------------------------------------------
//TextureMap
//-----------------------------------------------------------------------------
Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D ProjectorMap;
Texture2D ShadowMap;

TextureCube DynamicCubeMap;
TextureCube SkyCubeMap;

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

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Specular += val.Specular;
    result.Emissive += val.Emissive;
}

//-----------------------------------------------------------------------------
//ProjectorTexture
//-----------------------------------------------------------------------------
struct ProjectorDesc
{
    float4 Color;

    Matrix View;
    Matrix Projection;
};

cbuffer CB_Projector
{
    ProjectorDesc Projector;
};

void VS_Projector(inout float4 wvp, float4 oPosition)
{
    wvp = WorldPosition(oPosition);
    wvp = mul(wvp, Projector.View);
    wvp = mul(wvp, Projector.Projection);
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


//-----------------------------------------------------------------------------
//PointLighting
//-----------------------------------------------------------------------------
#define MAX_POINT_LIGHTS 256
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;

    float3 Position;
    float Range;

    float Intensity;
    float3 Padding;
};

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;

    PointLight PointLights[MAX_POINT_LIGHTS];
};

void ComputePointLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;

    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);

        [flatten]
        if (dist > PointLights[i].Range)
            continue;

        light /= dist;

        //Ambient
        result.Ambient = GlobalLight.Ambient * Material.Ambient * PointLights[i].Ambient;
        
        float lambert = dot(normalize(normal), light);
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (lambert > 0.0f)
        {
            //Diffuse
            result.Diffuse = Material.Diffuse * lambert * PointLights[i].Diffuse;

            //Specular
            [flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));

                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * PointLights[i].Specular;
            }
        }

        //Emissive
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(normalize(normal), E);
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - NdotE);

            result.Emissive = Material.Emissive * emissive * PointLights[i].Emissive;
        }


        //°¨¼â
        float temp = 1.0f / saturate(dist / PointLights[i].Range);
        float att = temp * temp * PointLights[i].Intensity;

        output.Ambient += result.Ambient * att;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;

    }//for(i)
}

//-----------------------------------------------------------------------------
//SpotLighting
//-----------------------------------------------------------------------------
#define MAX_SPOT_LIGHTS 256
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;

    float3 Position;
    float Range;

    float3 Direction;
    float Angle;

    float Intensity;
    float3 Padding;
};

cbuffer CB_SpotLights
{
    uint SpotLightCount;
    float3 CB_SpotLights_Padding;

    SpotLight SpotLights[MAX_SPOT_LIGHTS];
};

void ComputeSpotLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;

    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 light = SpotLights[i].Position - wPosition;
        float dist = length(light);

        [flatten]
        if (dist > SpotLights[i].Range)
            continue;

        light /= dist;

        //Ambient
        result.Ambient = GlobalLight.Ambient * Material.Ambient * SpotLights[i].Ambient;
        
        float lambert = dot(normalize(normal), light);
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (lambert > 0.0f)
        {
            //Diffuse
            result.Diffuse = Material.Diffuse * lambert * SpotLights[i].Diffuse;

            //Specular
            [flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));

                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * SpotLights[i].Specular;
            }
        }

        //Emissive
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(normalize(normal), E);
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - NdotE);

            result.Emissive = Material.Emissive * emissive * SpotLights[i].Emissive;
        }


        //°¨¼â & °­µµ
        float temp = pow(saturate(dot(-light, SpotLights[i].Direction)), 90.0f - SpotLights[i].Angle);
        float att = temp * (1.0f / max(1.0f - SpotLights[i].Intensity, 1e-6f));

        output.Ambient += result.Ambient * att;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;

    } //for(i)
}


float4 PS_Phong(MeshOutput input)
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);

    MaterialDesc output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;

    ComputeLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputePointLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputeSpotLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    return float4(MaterialToColor(result), 1.0f) + input.Color;
}

//-----------------------------------------------------------------------------
//Shadow
//-----------------------------------------------------------------------------
float4 PS_Shadow_Depth(MeshDepthOutput input) : SV_Target
{
    float depth = input.sPosition.z / input.sPosition.w;

    return float4(depth, depth, depth, 1);
}

float4 PS_Shadow(float4 sPosition, float4 color)
{
    float4 position = sPosition;
    position.xyz /= position.w;

    [flatten]
    if (position.x < -1.0f || position.x > 1.0f &&
        position.y < -1.0f || position.y > 1.0f &&
        position.z < 0.0f || position.z > 1.0f)
    {
        return color;
    }

    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f;

    float depth = 0.0f;
    float z = position.z - Shadow.Bias;
    float factor = 0.0f;

    if (Shadow.Quality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, position.xy).r;
        factor = (float) (depth >= z);
    }
    else if (Shadow.Quality == 1)
    {
        depth = z;
        factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, position.xy, depth).r;
    }
    else if (Shadow.Quality == 2)
    {
        depth = z;

        float2 size = 1.0f / Shadow.MapSize;
        float2 offsets[] =
        {
            float2(-size.x, -size.y), float2(0, -size.y), float2(+size.x, -size.y),
            float2(-size.x, 0.0), float2(0, 0), float2(+size.x, 0),
            float2(-size.x, +size.y), float2(0, +size.y), float2(+size.x, +size.y)
        };

        float2 uv = 0;
        float sum = 0;
        
        [unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = position.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
        }

        factor = sum / 9.0f;
    
    }
    

    factor = saturate(factor + depth);

    return float4(color.rgb * factor, 1);
}