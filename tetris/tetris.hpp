#pragma once

#include <cstdint>
#include <update_timer.hpp>

#include <graphical_application.hpp>

#include <list>
#include <memory>
#include <functional>

class Camera;
class Field;

class Tetris : public GraphicalApplication
{
public:
    Tetris();
    ~Tetris();

    void onKeyPressed(int key, int scancode, int action, int mods);

    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<IRenderer> m_renderer;
    std::shared_ptr<IPipeline> m_pipeline;

    bool m_rotate;
    int32_t m_dx;
    int32_t m_flushedTotal;

    UpdateTimer<TimeResolution> m_gameTimer;
    UpdateTimer<TimeResolution> m_moveTimer;
    UpdateTimer<TimeResolution> m_rotationTimer;

    std::shared_ptr<Field> m_field;
};
