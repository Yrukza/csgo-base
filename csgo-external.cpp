#include <iostream>
#include "Memory.h"
#include "Offsets.h"

__declspec(align(16)) struct Color
{
	constexpr Color(const float r, const float g, const float b, const float a = 1.f) noexcept :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};

int main()
{
	const auto mem = Memory("csgo.exe");
	std::cout << "[!] Csgo Found." << std::endl;

	const auto client = mem.GetModuleAddress("client.dll");
	std::cout << "client.dll -> " << "0x" << std::hex << client << std::dec << std::endl;

	constexpr const auto color = Color{ 1.f, 0.f, 1.f };

	for (int i = 0; i < 64; i++)
	{
		const auto entity = mem.Read<std::uintptr_t>(client + hazedumper::signatures::dwEntityList + i * 0x10);
		const auto health = mem.Read<std::int16_t>(entity + hazedumper::netvars::m_iHealth);
		const auto team = mem.Read<std::int16_t>(entity + hazedumper::netvars::m_iTeamNum);
		std::cout << "Player " << i << " Health: " << health << " Team: " << team << std::endl;
	}

	const auto glowObjectManager = mem.Read<std::uintptr_t>(client + hazedumper::signatures::dwGlowObjectManager);

	while (true)
	{
		for (int i = 0; i < 64; i++)
		{
			const auto entity = mem.Read<std::uintptr_t>(client + hazedumper::signatures::dwEntityList + i * 0x10);
			const auto health = mem.Read<std::int16_t>(entity + hazedumper::netvars::m_iHealth);
			if (!entity) { 
				continue;
			}

			if (health > 0)
			{
				const auto glowIndex = mem.Read<std::int32_t>(entity + hazedumper::netvars::m_iGlowIndex);

				mem.Write<Color>(glowObjectManager + (glowIndex * 0x38) + 0x8, color);

				mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
				mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x29, false);
			}
		}
		Sleep(.1);
	}
	
}