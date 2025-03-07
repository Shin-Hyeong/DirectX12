#pragma once

// 라이브러리 링커 : 해당 라이브러리를 사용하겠다고 연결 시킴
// - 솔루션 우클릭 -> 구성 속성 - VC++ 디렉터리 
//		-> 포함 디렉터리((SolutionDir)Engine), 라이브러리 디렉터리((SolutionDir)Output) 설정
// -- 포함 디텍터리 : 헤더파일 위치
// -- 라이브러리 디텍터리 : 빌드된 lib 위치
#pragma comment(lib, "Engine.lib")

// Pre Compilated Header(pch.h, stdafx.h) : 맨 처음에 한번 컴파일하여 필요한 곳에서 꺼내서 사용함
// - 자주 사용하는 기능들을 매번 파일마다 #include하는 것은 비효율적임 
// - 공용으로 많이 사용할 것은 한곳에 모아서 관리함
//
// PCH 설정 방법
// 솔루션 우클릭 -> 속성 -> 구성 속성 - C/C++ - 미리 컴파일된 헤더 -> 미리 컴파일된 헤더(미리 컴파일된 헤더 사용 함), 미리 컴파일된 헤더 파일(pch.h)로 설정
// 솔루션 우클릭 -> 속성 -> 상단에 구성(모든 구성), 플랫폼(모든 플랫폼)으로 설정
// pch.cpp 우클릭 -> 속성 -> 구성 속성 - C/C++ - 미리 컴파일된 헤더 -> 미리 컴파일된 헤더(만들기)

// #include <vector> -> EnginePch.h에서 선언됨
// #include <memory> -> EnginePch.h에서 선언됨

// Engine
#include "EnginePch.h"


// typedef 
// using namespace std; -> EnginePch.h에서 선언됨