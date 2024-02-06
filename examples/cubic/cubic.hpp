#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

class IModel;
class ITexture;
class Camera;
class Renderable;

class Hero;
class Map;

class Cubic : public GraphicalApplication
{
public:
    Cubic(CreateInfo createInfo);
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

    std::shared_ptr<IRenderer> m_renderer;
    std::shared_ptr<IPipeline> m_pipeline;

    std::unique_ptr<Hero> m_hero;
    std::unique_ptr<Map> m_map;

    std::shared_ptr<IModel> m_model;
    std::shared_ptr<ITexture> m_texture;
    std::shared_ptr<Renderable> m_renderable;
};
