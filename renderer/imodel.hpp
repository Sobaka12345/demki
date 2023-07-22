#pragma once

class RenderContext;

class IModel
{
public:
    virtual ~IModel(){};

    virtual void bind(const RenderContext& context) = 0;
    virtual void draw(const RenderContext& context) = 0;
};
