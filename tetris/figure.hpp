#pragma once

#include "position.hpp"

#include "../renderer/ubo_value.hpp"
#include "../renderer/renderable.hpp"

#include <functional>

class Block : public Renderable
{
public:
    Block(std::unique_ptr<vk::UBOValue<vk::UBOModel>> position3D);

    virtual void draw(VkCommandBuffer commandBuffer) const override;

    void move(int32_t dx, int32_t dy);
    void setPosition(Position position);
    Position position() const;

protected:
    Position m_position;
    std::unique_ptr<vk::UBOValue<vk::UBOModel>> m_position3D;
};

class Figure : public Renderable
{
public:
    static constexpr size_t s_blocksCount = 4;

public:
    Figure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
    void move(int32_t dx, int32_t dy);
    virtual void draw(VkCommandBuffer commandBuffer) const override;
    virtual void setModel(std::weak_ptr<Model> model) override;

protected:
    std::vector<Block> m_blocks;
    const vk::DescriptorSet* m_descriptorSet;
};

class LFigure : public Figure
{
public:
    LFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
};

class LRFigure : public Figure
{
public:
    LRFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
};

class ZFigure : public Figure
{
public:
    ZFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
};

class ZRFigure : public Figure
{
public:
    ZRFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
};

class OFigure : public Figure
{
public:
    OFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
};

class IFigure : public Figure
{
public:
    IFigure(const vk::DescriptorSet* descriptorSet, vk::IUBOProvider* uboProvider);
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

using FiguresMaker = FiguresFactory<IFigure, OFigure, LFigure, LRFigure, ZFigure, ZRFigure>;
