#pragma once

class OperationContext;

class IModel
{
public:
    virtual ~IModel(){};

    virtual void bind(const OperationContext& context) = 0;
    virtual void draw(const OperationContext& context) = 0;
};
