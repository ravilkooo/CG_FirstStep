struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer CBuf
{
    row_major float4x4 wvpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.pos = mul(float4(input.pos, 1.0), wvpMat);
    output.col = input.col;
	
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.col * (1 - input.pos.z); //sqrt
    return col;
}

void Subdivide(PS_IN inVerts[3], out PS_IN outVerts[6])
{
    PS_IN m[3];
    // Compute edge midpoints.
    m[0].pos = 0.5f * (inVerts[0].pos + inVerts[1].pos);
    m[1].pos = 0.5f * (inVerts[1].pos + inVerts[2].pos);
    m[2].pos = 0.5f * (inVerts[2].pos + inVerts[0].pos);
    // Project onto unit sphere
    m[0].pos = normalize(m[0].pos);
    m[1].pos = normalize(m[1].pos);
    m[2].pos = normalize(m[2].pos);
    // Derive normals.
    // Interpolate texture coordinates.
    outVerts[0] = inVerts[0];
    outVerts[1] = m[0];
    outVerts[2] = m[2];
    outVerts[3] = m[1];
    outVerts[4] = inVerts[2];
    outVerts[5] = inVerts[1];
};