#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f

float mod289(float x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float2 mod289(float2 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float3 mod289(float3 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float4 mod289(float4 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float permute(float x)
{
    return mod289(
        x * x * 34.0 + x
    );
}
 
float3 permute(float3 x)
{
    return mod289(
        x * x * 34.0 + x
    );
}
 
float4 permute(float4 x)
{
    return mod289(
        x * x * 34.0 + x
    );
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

// 2D

float snoise(float2 v)
{
    const float4 C = float4(
        0.211324865405187, // (3.0-sqrt(3.0))/6.0
        0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
     -0.577350269189626, // -1.0 + 2.0 * C.x
        0.024390243902439 // 1.0 / 41.0
    );
 
// First corner
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);
 
// Other corners
    // float2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    // Lex-DRL: afaik, step() in GPU is faster than if(), so:
    // step(x, y) = x <= y
 
    // Optimization hint Dolkar
    //int xLessEqual = step(x0.x, x0.y); // x <= y ?
    //int2 i1 =
    //    int2(1, 0) * (1 - xLessEqual) // x > y
    //    + int2(0, 1) * xLessEqual // x <= y
    //;
    //float4 x12 = x0.xyxy + C.xxzz;
    //x12.xy -= i1;
 
    float4 x12 = x0.xyxy + C.xxzz;
    int2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    x12.xy -= i1;
 
// Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float3 p = permute(
        permute(
                i.y + float3(0.0, i1.y, 1.0)
        ) + i.x + float3(0.0, i1.x, 1.0)
    );
 
    float3 m = max(
        0.5 - float3(
            dot(x0, x0),
            dot(x12.xy, x12.xy),
            dot(x12.zw, x12.zw)
        ),
        0.0
    );
    m = m * m;
    m = m * m;
 
// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
 
    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
 
// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
 
// Compute final noise value at P
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}


// 3D

float snoise(float3 v)
{
    const float2 C = float2(
		0.166666666666666667, // 1/6
		0.333333333333333333 // 1/3
	);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);
	
// First corner
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);
	
// Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);
	
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    float3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y
	
// Permutations
    i = mod289(i);
    float4 p = permute(
		permute(
			permute(
					i.z + float4(0.0, i1.z, i2.z, 1.0)
			) + i.y + float4(0.0, i1.y, i2.y, 1.0)
		) + i.x + float4(0.0, i1.x, i2.x, 1.0)
	);
	
// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1/7
    float3 ns = n_ * D.wyz - D.xzx;
	
    float4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)
	
    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_); // mod(j,N)
	
    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);
	
    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
	
	//float4 s0 = float4(lessThan(b0,0.0))*2.0 - 1.0;
	//float4 s1 = float4(lessThan(b1,0.0))*2.0 - 1.0;
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, 0.0);
	
    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
	
    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);
	
//Normalise gradients
    float4 norm = taylorInvSqrt(float4(
		dot(p0, p0),
		dot(p1, p1),
		dot(p2, p2),
		dot(p3, p3)
	));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
	
// Mix final noise value
    float4 m = max(
		0.6 - float4(
			dot(x0, x0),
			dot(x1, x1),
			dot(x2, x2),
			dot(x3, x3)
		),
		0.0
	);
    m = m * m;
    return 42.0 * dot(
		m * m,
		float4(
			dot(p0, x0),
			dot(p1, x1),
			dot(p2, x2),
			dot(p3, x3)
		)
	);
};


float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

float some_noise(float3 x)
{
    // The noise function returns a value in the range -1.0f -> 1.0f
    float3 p = floor(x);
    float3 f = frac(x);
     
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 57.0 + 113.0 * p.z;
     
    return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
               lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
               lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
               lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}


struct VS_IN
{
    float3 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 screen_pos : SV_POSITION;
    float3 pos : POSITION0;
    float4 col : COLOR;
};

cbuffer CBuf
{
    row_major float4x4 wMat;
    row_major float4x4 wMatInvTranspose;
    row_major float4x4 vpMat;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN) 0;
	
    output.screen_pos = mul(mul(float4(input.pos, 1.0), wMat), vpMat);
    output.pos = input.pos;
    output.col = input.col;
	
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = snoise(input.pos);
    col = col > 0.85f ? 1 : 0;
    float band_width = 0.1;
    
    //atan2(input.pos.x, input.pos.z);
    float rand_val = snoise(0.5 * abs(float2(input.pos.x, input.pos.z) / length(input.pos.xz)));
    float delta_1 = rand_val * band_width * 0.8;
    float tg = abs(input.pos.y / length(input.pos.xz));
    if (tg < band_width - delta_1)
    {
        float white_part = 0.6;
        if (tg < white_part*(band_width - delta_1))
        {
            float4 col_2 = snoise(0.2 * (input.pos));
            col = max(col_2 * (1 - tg / (white_part * (band_width - delta_1))), col);
        }
        
        
        float4 col_1 = snoise(0.7 * (input.pos));
        col_1.r = 0;
        col_1.g *= 0.8;
        col = max(col_1 * (1 - tg / (band_width - delta_1)), col);
        // col = col_2;
    }
    
    return col;
}
