
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
