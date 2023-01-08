// triangle_gs
// Geometry shader that generates a triangle for every vertex.

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer grassMoveBuffer : register(b1)
{

	float slider;
	float3 camPos;
	float time;
	float speed;
	float limit;
	float lHight;
	float uHeight;
	float3 padding;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD1;
};

int rand(float n)
{
	// <<, ^ and & require GL_EXT_gpu_shader4.
	//n = (n << 13.f) ^ n;
	return (n * (n * n * 15731 + 789221) + 1376312589);
}

float rand3(float2 co)
{
	return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

float3 rotate(float3 pos, float angl)
{
	//position to rotate
	float3 p = pos;

	//angle in rads
	float angle = rand(angl) * 3.14 / 180.0;

	//set rotation axis
	float x = 0.0;
	float y = 1.0;
	float z = 0.0;

	float3 h;
	h.x = p.x * (x * x * (1.0f - cos(angle)) + cos(angle))
		+ p.y * (x * y * (1.0f - cos(angle)) + z * sin(angle))
		+ p.z * (x * z * (1.0f - cos(angle)) - y * sin(angle));

	h.y = p.x * (y * x * (1.0f - cos(angle)) - z * sin(angle))
		+ p.y * (y * y * (1.0f - cos(angle)) + cos(angle))
		+ p.z * (y * z * (1.0f - cos(angle)) + x * sin(angle));

	h.z = p.x * (z * x * (1.0f - cos(angle)) + y * sin(angle))
		+ p.y * (z * y * (1.0f - cos(angle)) - x * sin(angle))
		+ p.z * (z * z * (1.0f - cos(angle)) + cos(angle));

	return h;
}


[maxvertexcount(8)]
void main(point InputType input[1], inout TriangleStream<OutputType> quadStream)
{
	float t = float((input[0].position.x / 8 + input[0].position.y / 4 + input[0].position.z / 2) * 191);
	float sit = cos(time * speed * rand3(t)) / limit;
	float sitx = sin(time * speed * rand3(t * 42)) / 8;

	float height = 0.21f;
	float hHeight = height / 2;
	float bWidth = 0.042f;
	float m1Width = 0.033f;
	float m2Width = 0.024f;
	float tWidth = 0.01f;
	float hWidth = bWidth / 2;

	float4 colour = texture0.SampleLevel(sampler0, input[0].tex, 0);

	float start = colour.y * 8;

	input[0].position.x /= 4;
	input[0].position.z /= 4;

	float3 grass_positions[8] =
	{
		float3(-bWidth / 2, start, 0),
		float3(bWidth / 2, start, 0),
		float3(-m1Width / 2 + sitx / 3, start + height / 3, cos(time * speed) / (limit * 6)),
		float3(m1Width / 2 + sitx / 3, start + height / 3, cos(time * speed) / (limit * 6)),
		float3(-m2Width / 2 + sitx / 2, start + height / 2, cos(time * speed) / (limit * 3)),
		float3(m2Width / 2 + sitx / 2, start + height / 2, cos(time * speed) / (limit * 3)),
		float3(-tWidth / 2 + sitx, start + height, cos(time * speed) / (limit * 1)),
		float3(tWidth / 2 + sitx, start + height, cos(time * speed) / (limit * 1))
	};

	OutputType output;

	if (start > lHight && start < uHeight)
	{
		matrix w = mul(worldMatrix, viewMatrix);
		w = mul(w, projectionMatrix);

		float t = float((input[0].position.x + input[0].position.y + input[0].position.z) / 215);

		for (int i = 0; i < 8; i++)
		{
			output.position = mul(float4(rotate(grass_positions[i].xyz, t), 1.0f) + input[0].position, w);
			output.depthPosition = output.position;
			quadStream.Append(output);
		}
	}
	quadStream.RestartStrip();
}