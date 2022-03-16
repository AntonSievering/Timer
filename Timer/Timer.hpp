#pragma once

#include <chrono>
#include <cstdint>
#include <functional>


class timer
{
public:
	using clock = std::chrono::system_clock;
	using time_point = std::chrono::time_point<clock>;

private:
	time_point m_tpStart{};

public:
	timer() noexcept = default;

public:
	timer start() noexcept
	{
		m_tpStart = clock::now();
		return *this;
	}

	float getElapsedTime() const noexcept
	{
		int64_t nNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - m_tpStart).count();
		return (float)nNanos / 1e9f;
	}
};

class scoped_timer
{
protected:
	float &m_fReturnValue;
	timer m_timer{};

public:
	scoped_timer() noexcept = delete;

	scoped_timer(float &returnValue) noexcept
		: m_fReturnValue(returnValue)
	{
		m_timer.start();
	}

	virtual ~scoped_timer() noexcept
	{
		m_fReturnValue = m_timer.getElapsedTime();
	}

public:
	float getCurrentTime() const noexcept
	{
		return m_timer.getElapsedTime();
	}

	void reset() noexcept
	{
		m_timer.start();
	}
};


class chained_scoped_timer final : public scoped_timer
{
public:
	chained_scoped_timer() noexcept = delete;

	chained_scoped_timer(float &returnValue) noexcept
		: scoped_timer(returnValue) {}

	~chained_scoped_timer() noexcept override
	{
		m_fReturnValue += m_timer.getElapsedTime();
	}
};


class time_accumulator
{
private:
	float m_fTotalTime = 0.0f;

public:
	time_accumulator() noexcept = default;

public:
	void add_time(const float &time) noexcept
	{
		m_fTotalTime += time;
	}

	float get_time() const noexcept
	{
		return m_fTotalTime;
	}

	void reset() noexcept
	{
		m_fTotalTime = 0.0f;
	}

public:
	template <class RetType, class ... Args>
	RetType measure(RetType(*fn)(Args ...), Args && ... args) noexcept
	{
		chained_scoped_timer timer(m_fTotalTime);
		return fn(args ...);
	}

	template <class RetType, class ObjType, class ... Args>
	RetType measure(RetType(ObjType::*fn)(Args ...), ObjType *obj, Args && ... args) noexcept
	{
		chained_scoped_timer timer(m_fTotalTime);
		return (obj->*fn)(args ...);
	}
};
