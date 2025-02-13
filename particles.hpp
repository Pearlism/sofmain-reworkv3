#include "C:\Users\419\Desktop\Fortnite-ExternalBase-main\imgui\imgui.h"
#include <vector>
#include <random>
#include <cmath>  
using namespace std;

class M_Particle_System {
public:
    enum class prt_type {
        snow,
        rain,
        ash
    };

    void render(prt_type type);

private:
    struct Particle {
        float x = 0.0f, y = 0.0f;
        float fallSpeed = 0.0f, driftSpeed = 0.0f;
        uint8_t alpha = 0;
        int width = 0, height = 0;
        float size = 1.0f;
    };

    std::vector<Particle> prts;

    Particle generate(prt_type type, bool firstTime = false);
    inline static int rand_int(int min, int max);
};

int M_Particle_System::rand_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(gen);
}

M_Particle_System::Particle M_Particle_System::generate(prt_type type, bool firstTime) {
    Particle p;
    auto io = ImGui::GetIO();
    p.x = static_cast<float>(rand_int(-static_cast<int>(io.DisplaySize.x / 2), static_cast<int>(io.DisplaySize.x)));
    p.y = static_cast<float>(rand_int(firstTime ? -static_cast<int>(io.DisplaySize.y * 2) : -100, -50));

    switch (type) {
    case prt_type::snow:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(5, 50) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(50, 200));
        p.size = rand_int(2, 5);
        break;
    case prt_type::rain:
        p.fallSpeed = rand_int(300, 600) / 100.0f;
        p.driftSpeed = rand_int(1, 10) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(100, 200));
        p.width = 1;
        p.height = rand_int(5, 10);
        break;
    case prt_type::ash:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(30, 100) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(50, 150));
        p.width = rand_int(2, 3);
        p.height = rand_int(4, 8);
        break;
    }
    return p;
}

void M_Particle_System::render(prt_type type) {
    float deltaTime = ImGui::GetIO().DeltaTime;
    if (prts.empty()) {
        prts.reserve(1400);
        for (int i = 0; i < 1400; ++i) {
            prts.push_back(generate(type, true));
        }
    }

    for (auto& p : prts) {
        if (p.y > ImGui::GetIO().DisplaySize.y + 50) {
            p = generate(type);
            continue;
        }

        p.x += p.driftSpeed * deltaTime * 100.0f;
        p.y += p.fallSpeed * deltaTime * 100.0f;


        if (p.y > ImGui::GetIO().DisplaySize.y * 0.9f) {
            p.alpha = static_cast<uint8_t>(max(0, static_cast<int>(p.alpha) - 2));
            p.size *= 0.95f;
        }


        ImU32 color = ImColor(255, 51, 153, p.alpha);


        ImVec2 pos(p.x, p.y);
        float radius = p.size * 0.5f;
        ImGui::GetBackgroundDrawList()->AddCircleFilled(pos, radius, color);
    }
}

namespace M { inline M_Particle_System Particle; }
