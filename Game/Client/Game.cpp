#include "pch.h"
#include "Game.h"
#include "Engine.h"

// Engine을 통해서 Class를 찾음 
shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();

void Game::BeginPlay(const WindowInfo& info)
{
	GEngine->Init(info);

	vector<Vertex> vec(3);
	// window 창의 정중앙이 0.0f 기준점
	// X : 기준점 왼쪽으로 가면 -, 오른쪽으로 가면 +
	// Y : 기준점 위로으로 가면 +, 아래로 가면 -
	// Z : 3D로 보았을  앞으로 가면 +, 뒤로가면 -
	vec[0].pos = Vec3(0.f, 0.5f, 0.5f);			// XYZ
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);	// RED
	vec[1].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.0f, 0.f, 1.f);	// GREEN
	vec[2].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);	// BLUE

	// vec의 정점 정보를 _vertexBuffer을 통해 GPU에 넘김
	mesh->Init(vec);

	// Init(Shader 파일이 있는 위치)
	// Shader 생성     
	shader->Init(L"..\\Resources\\Shader\\Default.hlsli");;

	// 동기화
	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Tick()
{
	// GEngine->Render();
	GEngine->RenderBegin();

	shader->Update();

	{
		Transform t;
		// Shader의 Offset1과 Offset2에 적용됨
		// x좌표로 0.75만큼 이동하고 R값을 0.75증가시킴
		t.offset = Vec4(0.75f, 0.f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}

	{
		Transform t;
		// Shader의 Offset1과 Offset2에 적용됨
		// Y좌표로 0.75만큼 이동하고 G값을 0.75증가시킴
		t.offset = Vec4(0.f, 0.75f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}

	GEngine->RenderEnd();
}
