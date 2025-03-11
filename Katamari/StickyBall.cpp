#include "StickyBall.h"

StickyBall::StickyBall()
{
	UINT sliceCount = 10;
	UINT elevationCount = 5;
	XMFLOAT4 col(0.9f, 0.0f, 0.0f, 1.0f);
	CreateSimpleSphereMesh(radius, sliceCount, elevationCount, col, &vertices, &verticesNum, &indices, &indicesNum);
	for (size_t i = 0; i < verticesNum; i++)
	{
		vertices[i].pos = Vector3::Transform(vertices[i].pos, Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, XM_PIDIV2));
	}
	//auto col_2 = XMFLOAT4(0.5f + 0.5f * col.x, 0.5f + 0.5f * col.y, 0.5f + 0.5f * col.z, 1.0f);
	auto col_2 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < elevationCount * 2 + 1; i += 2)
	{
		for (size_t j = 0; j < sliceCount; j++)
		{
			vertices[1 + i * sliceCount + j].color = col_2;
		}
	}
	worldMat = Matrix::CreateTranslation(position);
}

Vector3 StickyBall::GetCenterLocation()
{
	return Vector3();
}

void StickyBall::PushForward(float deltaTime)
{
	velocity = min(max_velocity, velocity + deltaTime * acceleration);
	spin_speed = velocity / radius;
}

void StickyBall::AddTurn(float direction, float deltaTime)
{
	//curr_rotation = min(max_rotation, curr_rotation + direction * deltaTime * rotation_speed);
	rotation_speed = max(-max_rotation_speed, min(max_rotation_speed, rotation_speed + direction * 2 * deltaTime * rot_acceleration));
	curr_rotation += rotation_speed * deltaTime;
}

void StickyBall::SlowDown(float deltaTime)
{
	velocity = max(0.0, velocity + deltaTime * acceleration);
	spin_speed = velocity / radius;
}

Vector3 StickyBall::GetMoveDir()
{
	return Vector3(velocity * cosf(curr_rotation), 0.0, velocity * sinf(curr_rotation));
}

void StickyBall::Update(float deltaTime)
{
	float rotSlow = deltaTime * rot_acceleration;
	if (abs(curr_rotation) < deltaTime * rotSlow)
	{
		rotation_speed = -curr_rotation / deltaTime;
	}
	else
	{
		rotation_speed -= rotSlow * (curr_rotation > 0.0f ? 1.0f : -1.0f);
	}
	//curr_rotation = 0.0f;

	Matrix rotTransform = Matrix::CreateFromYawPitchRoll(deltaTime * rotation_speed, 0.0f, deltaTime * spin_speed);
	worldMat = rotTransform * worldMat * Matrix::CreateTranslation(GetMoveDir());


	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}