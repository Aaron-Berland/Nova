#pragma once
#ifndef TIMESTEP_H
#define TIMESTEP_H

namespace Nova
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_time(time) {}

		operator float() { return m_time; }
		float GetSeconds() const { return m_time; }
		float GetMiliSeconds() const { return m_time * 1000.0f; }

	private:
		float m_time;
	};
}
#endif