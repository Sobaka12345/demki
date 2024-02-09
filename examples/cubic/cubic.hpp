#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

namespace renderer {
class IModel;
class ITexture;
class Camera;
class Renderable;
}

class Hero;
class Map;

class Cubic : public engine::GraphicalApplication
{
public:
    Cubic(int& argc, char** argv);
    ~Cubic();

private:
    void onKeyPressed(int key, int scancode, int action, int mods);

    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    int m_movement;

    double xCursorPrev;
    double yCursorPrev;

    UpdateTimer<TimeResolution> m_jumpTimer;
    UpdateTimer<TimeResolution> m_placeBlockTimer;
    UpdateTimer<TimeResolution> m_turnTimer;
    UpdateTimer<TimeResolution> m_movementTimer;

    std::shared_ptr<renderer::IRenderer> m_renderer;
    std::shared_ptr<renderer::IPipeline> m_pipeline;

    std::unique_ptr<Hero> m_hero;
    std::unique_ptr<Map> m_map;

    std::shared_ptr<renderer::IModel> m_model;
    std::shared_ptr<renderer::ITexture> m_texture;
    std::shared_ptr<renderer::Renderable> m_renderable;
};
