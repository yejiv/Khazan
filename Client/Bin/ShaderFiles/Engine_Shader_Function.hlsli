
uint pcg_hash(uint seed)
{
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand_float(uint seed)
{
    return (float) pcg_hash(seed) / 4294967295.0f;
}

float rand_between(float minVal, float maxVal, uint ID)
{
    float t = rand_float(ID);
    return lerp(minVal, maxVal, t);
}

float GetAlphaFadeInOut(float t)
{
    float angle = t * 2.0f * 3.141592;
    return 0.5f * (1.0f - cos(angle));
}


float4 Dissolve(float fDecreaseAlpha, float noise, float EdgeWidth, float4 EdgeColor, float4 InColor)
{
    float4 rt = InColor;
        
    if (noise < fDecreaseAlpha)
    {
        rt.a = 0.f;
        return rt;
    }
    
    float edgeStart = fDecreaseAlpha;
    float edgeEnd = fDecreaseAlpha + EdgeWidth;
    float edgeFactor = smoothstep(edgeStart, edgeEnd, noise);
    rt = lerp(EdgeColor * 2.f, InColor, edgeFactor);
    
    return rt;
}
