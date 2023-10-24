#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <numbers>
#include <cmath>
#include <vector>
#include <optional>
#include <list>

HANDLE TargetProcess;

#define dwEntityList 0x17888D8
#define dwGlobalVars 0x1690D18
#define dwLocalPlayerController 0x17D7158
#define dwLocalPlayerPawn 0x1875C48
#define dwViewAngles 0x18D5700
#define dwViewMatrix 0x1876730
#define dwPlantedC4 0x1880DE0

#define m_iPawnHealth 0x808
#define m_hPlayerPawn 0x7FC
#define m_sSanitizedPlayerName 0x720
#define m_iTeamNum 0x3BF
#define m_vOldOrigin 0x1214
#define m_pGameSceneNode 0x310

bool isWindowTransparent = true;


uintptr_t GetModuleBaseAddress(DWORD dwPid, const wchar_t* moduleName) {
    uintptr_t dwBase = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPid);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32W);
        if (Module32FirstW(hSnapshot, &ModuleEntry32)) {
            do {
                if (!wcscmp(ModuleEntry32.szModule, moduleName)) {
                    dwBase = (uintptr_t)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return dwBase;
}



template <typename T>
T RPM(SIZE_T address) {
    T buffer;
    ReadProcessMemory(TargetProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
    return buffer;
}

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3
{
	// constructor
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	// operator overloads
	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept
	{
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept
	{
		return Vector3{ x * factor, y * factor, z * factor };
	}

	constexpr const bool operator>(const Vector3& other) const noexcept {
		return x > other.x && y > other.y && z > other.z;
	}

	constexpr const bool operator>=(const Vector3& other) const noexcept {
		return x >= other.x && y >= other.y && z >= other.z;
	}

	constexpr const bool operator<(const Vector3& other) const noexcept {
		return x < other.x&& y < other.y&& z < other.z;
	}

	constexpr const bool operator<=(const Vector3& other) const noexcept {
		return x <= other.x && y <= other.y && z <= other.z;
	}

	// utils
	constexpr const Vector3& ToAngle() const noexcept
	{
		return Vector3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f };
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float length2d() const {
		return std::sqrt(x * x + y * y);
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	Vector3 world_to_screen(view_matrix_t matrix) const {
		float _x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3];
		float _y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3];

		float w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3];

		float inv_w = 1.f / w;
		_x *= inv_w;
		_y *= inv_w;

		float x = 3440 * .5f;
		float y = 1440 * .5f;

		x += 0.5f * _x * 3440 + 0.5f;
		y -= 0.5f * _y * 1440 + 0.5f;

		return { x, y, w };
	}

	// struct data
	float x, y, z;
};


struct Vector2 {
	float x, y;
};

bool wrld_to_screen(const Vector3& pos, Vector3& out, view_matrix_t matrix) {
	out.x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	out.y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

	if (w < 0.01f)
		return false;

	float inv_w = 1.f / w;
	out.x *= inv_w;
	out.y *= inv_w;

	float x = 3440 * .5f;
	float y = 1440 * .5f;

	x += 0.5f * out.x * 3440 + 0.5f;
	y -= 0.5f * out.y * 1440 + 0.5f;

	out.x = x;
	out.y = y;

	return true;
}

