#pragma once
#include "X11Window.h"
#include <cmath>
#include <random>
#include <thread>

constexpr int MIN_SNOWFLAKE_RADIUS = 3;
constexpr int MAX_SNOWFLAKE_RADIUS = 10;
constexpr int MIN_SNOWFLAKE_SPEED = 40;
constexpr int MAX_SNOWFLAKE_SPEED = 200;
constexpr int FRAME_DELAY_MS = 20;

constexpr double SINE_FREQUENCY = 0.01f;
constexpr double SINE_AMPLITUDE = 5.0f;

struct Snowflake
{
	double x;
	double y;
	int radius;
	int speed;
	double timeOffset;
};

class SnowfallApp
{
public:
	explicit SnowfallApp(const int snowflakesCount)
	{
		InitializeSnowflakes(snowflakesCount);
	}

	void Run()
	{
		// steady clock better
		using clock = std::chrono::high_resolution_clock;
		auto lastFrameTime = clock::now();

		while (m_window.ProcessEvents())
		{
			auto currentFrameTime = clock::now();
			const auto deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
			lastFrameTime = currentFrameTime;

			Update(deltaTime);
			Render();
			std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY_MS));
		}
	}

private:
	void InitializeSnowflakes(const int snowflakesCount)
	{
		std::uniform_int_distribution randomX(0, m_window.GetWidth() - 1);
		std::uniform_int_distribution randomY(0, m_window.GetHeight() - 1);
		std::uniform_int_distribution randomRadius(MIN_SNOWFLAKE_RADIUS, MAX_SNOWFLAKE_RADIUS);
		std::uniform_real_distribution<> randomTimeOffset(0.0f, 2.0f * M_PI);

		m_snowflakes.resize(snowflakesCount);
		for (auto& [x, y, radius, speed, timeOffset] : m_snowflakes)
		{
			x = randomX(m_randomGen);
			y = randomY(m_randomGen);
			radius = randomRadius(m_randomGen);
			timeOffset = randomTimeOffset(m_randomGen);

			speed = MIN_SNOWFLAKE_SPEED + (radius - MIN_SNOWFLAKE_RADIUS) * (MAX_SNOWFLAKE_SPEED - MIN_SNOWFLAKE_SPEED) / (MAX_SNOWFLAKE_RADIUS - MIN_SNOWFLAKE_RADIUS);
		}
	}

	void Update(const float deltaTime)
	{
		std::uniform_int_distribution randomX(0, m_window.GetWidth() - 1);
		for (auto& [x, y, radius, speed, timeOffset] : m_snowflakes)
		{
			y += static_cast<int>(deltaTime * static_cast<float>(speed));

			// сделать в зависимости от размера снежинки
			x += static_cast<int>(SINE_AMPLITUDE * std::sin(SINE_FREQUENCY * y + timeOffset));

			if (y - radius > m_window.GetHeight())
			{
				y = 0 - radius;
				x = randomX(m_randomGen);
				timeOffset = static_cast<float>(m_randomGen() % 100) / 100.0f * 2.0f * M_PI;
			}
		}
	}

	void Render() const
	{
		m_window.Clear();
		m_window.DrawBackground();
		DrawSnowflakes();
	}

	void DrawSnowflakes() const
	{
		for (const auto& flake : m_snowflakes)
		{
			if (IsFlakeInWindow(flake))
			{
				m_window.DrawCircle(static_cast<int>(flake.x), static_cast<int>(flake.y), flake.radius);
			}
		}
	}

	[[nodiscard]] bool IsFlakeInWindow(const Snowflake& flake) const
	{
		return flake.y - flake.radius < m_window.GetHeight() && flake.x - flake.radius < m_window.GetWidth();
	}

	X11Window m_window;
	std::vector<Snowflake> m_snowflakes;
	std::mt19937 m_randomGen{ std::random_device{}() };
};
