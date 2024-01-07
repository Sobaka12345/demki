#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

class IModel;
class ITexture;
class Camera;
class Renderable;

class Dummy : public GraphicalApplication
{
public:
    Dummy(CreateInfo createInfo);
    ~Dummy();

private:
    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    UpdateTimer<TimeResolution> m_timer;

    std::shared_ptr<IRenderer> m_renderer;
    std::shared_ptr<IPipeline> m_pipeline;

    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<IModel> m_model;
    std::shared_ptr<ITexture> m_texture;
    std::shared_ptr<Renderable> m_renderable;
};
