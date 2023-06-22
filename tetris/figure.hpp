#pragma once

#include "field.hpp"
#include "position.hpp"

#include <ubo_value.hpp>
#include <renderable.hpp>

#include <functional>

class Block : public Renderable
{
public:
    Block(std::unique_ptr<vk::UBOValue<vk::UBOModel>> position3D);

    virtual void draw(const vk::CommandBuffer& commandBuffer) const override;
    bool canMove(int32_t dx, int32_t dy);
    bool canSetPosition(Position position);
    void move(int32_t dx, int32_t dy);
    void setPosition(Position position);
    Position position() const;

private:

protected:
    Position m_position;
    std::unique_ptr<vk::UBOValue<vk::UBOModel>> m_position3D;
};

class Figure : public Renderable
{
    static Position s_blocksPositions[Field::s_height][Field::s_width];

public:
    static constexpr size_t s_blocksCount = 4;

public:
    Figure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    const std::array<std::shared_ptr<Block>, s_blocksCount>& blocks() const;
    bool hitTest(Position pos) const;

    virtual bool tryMove(int32_t dx, int32_t dy);
    virtual bool tryRotate();
    virtual Position rotationAnchor() const = 0;
    virtual void draw(const vk::CommandBuffer& commandBuffer) const override;
    virtual void setModel(std::weak_ptr<Model> model) override;

protected:
    const Field* m_field;
    std::array<std::shared_ptr<Block>, s_blocksCount> m_blocks;
};

class LFigure : public Figure
{
public:
    LFigure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

class LRFigure : public Figure
{
public:
    LRFigure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

class ZFigure : public Figure
{
public:
    ZFigure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

class ZRFigure : public Figure
{
public:
    ZRFigure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

class OFigure : public Figure
{
public:
    OFigure(const Field* field,
        const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual bool tryRotate() override;
    virtual Position rotationAnchor() const override;
};

class IFigure : public Figure
{
public:
    IFigure(const Field* field,
            const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

class TFigure : public Figure
{
public:
    TFigure(const Field* field,
            const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    virtual Position rotationAnchor() const override;
};

template <typename ...Args>
struct FiguresFactory
{
    template <typename T, typename ...MakeArgs>
    static std::shared_ptr<Figure> make(MakeArgs ...args) { return std::make_shared<T>(args...); }

    static constexpr size_t figuresCount() { return sizeof...(Args); }

    template<typename ...MakeArgs>
    static std::shared_ptr<Figure> createRandomFigure(MakeArgs ...args)
    {
        static const std::array<std::function<std::shared_ptr<Figure> (MakeArgs...)>, figuresCount()> makers = {
            std::function{make<Args, MakeArgs...>}...
        };

        return makers[std::rand() % figuresCount()](args...);
    }
};

using FiguresMaker = FiguresFactory<IFigure, OFigure, LFigure, LRFigure, ZFigure, ZRFigure, TFigure>;
