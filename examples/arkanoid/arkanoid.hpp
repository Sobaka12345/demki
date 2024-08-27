#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

namespace renderer {
class IMesh;
class ITexture;
class Camera;
class Renderable;
}

class Arkanoid : public engine::GraphicalApplication
{
public:
    Arkanoid(int& argc, char** argv);
    ~Arkanoid();

protected:
    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    UpdateTimer<TimeResolution> m_movementTimer;

    std::shared_ptr<renderer::IRenderer> m_renderer;
    std::shared_ptr<renderer::IPipeline> m_pipeline;
};
