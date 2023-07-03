#pragma once

template <typename T>
class UpdateTimer
{
public:
    UpdateTimer(int32_t interval = T::den)
        : m_interval(interval)
        , m_timer(0)
        , m_speedUpCoeff(1.0f)
    {}

    bool timePassed(int64_t dt)
    {
        return (m_timer += dt) > m_interval / m_speedUpCoeff ? m_timer = 0, true : false;
    }

    template <typename ResolutionType = T>
    int64_t interval() const
    {
        return (m_interval / double(T::den)) * ResolutionType::den;
    }

    void setNormalSpeed() { m_speedUpCoeff = 1.0f; }

    void setSpeedUp(float coeff) { m_speedUpCoeff = coeff; }

    void setIntervalMS(int64_t intervalMS) { setInterval<std::milli>(intervalMS); }

    template <typename ResolutionType = T>
    void setInterval(int64_t interval)
    {
        m_interval = (interval / double(ResolutionType::den)) * T::den;
    }

    template <typename ResolutionType = T>
    void reset(int64_t initialDelay = 0)
    {
        m_timer = -(initialDelay / double(ResolutionType::den)) * T::den;
    }

private:
    int64_t m_timer;
    int64_t m_interval;
    float m_speedUpCoeff;
};
