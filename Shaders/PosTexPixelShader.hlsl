Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_OUTPUT mainPS(PS_INPUT input) : SV_TARGET
{
    // output.depthPosition = output.position;
    
    PS_OUTPUT output;

    //output.normal = input.normal;
    //output.texcoord = input.texcoord;
    float3 color = g_texture0.Sample(g_sampler, input.texcoord).rgb;
    output.color = float4(color, 1.0f);
    //output.color = input.color;

    return output;
}