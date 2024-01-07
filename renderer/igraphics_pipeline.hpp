#pragma once

#include <ipipeline.hpp>
#include <type_utils.hpp>

#include <boost/pfr.hpp>

class IGraphicsPipeline : virtual public IPipeline
{
public:
    class CreateInfo : public IPipeline::CreateInfo<CreateInfo>
    {
    public:
        struct Binding
        {
            enum Rate
            {
                VERTEX
            };

            uint32_t binding = 0;
            uint32_t stride = 0;
            Rate inputRate = VERTEX;
        };

        struct Attribute
        {
            enum Format
            {
                INVALID = -1,
                FLOAT = 0,
                VEC2,
                VEC3,
                VEC4,
                COUNT
            };

            template <typename T>
            static Format attrubuteFormat(T& valueT)
            {
                if constexpr (std::is_same_v<T, glm::float32>) return FLOAT;
                if constexpr (std::is_same_v<T, glm::vec2>) return VEC2;
                if constexpr (std::is_same_v<T, glm::vec3>) return VEC3;
                if constexpr (std::is_same_v<T, glm::vec4>) return VEC4;
                return INVALID;
            }

            uint32_t location = 0;
            uint32_t binding = 0;
            Format format = FLOAT;
            uint32_t offset = 0;
        };

        enum PrimitiveTopology
        {
            POINTS,
            LINES,
            TRIANGLES
        };

        enum CullMode
        {
            FRONT,
            BACK,
            FRONT_AND_BACK
        };

        enum FrontFace
        {
            CLOCKWISE,
            COUNTER_CLOCKWISE
        };

        enum PolygonMode
        {
            FILL,
            LINE,
            POINT
        };

        enum SampleShading
        {
            SS_0_PERCENT,
            SS_20_PERCENT,
            SS_40_PERCENT,
            SS_60_PERCENT,
            SS_80_PERCENT,
            SS_100_PERCENT,
        };

    public:
        template <typename T>
        CreateInfo& addInput()
        {
            T val;
            m_bindings.push_back(Binding{
                .binding = static_cast<uint32_t>(m_bindings.size()),
                .stride = sizeof(decltype(val)),
                .inputRate = Binding::VERTEX,
            });

            uint32_t j = 0;
            boost::pfr::for_each_field(val, [&](auto& subVal) {
                m_attributes.push_back(Attribute{
                    .location = j++,
                    .binding = m_bindings.back().binding,
                    .format = Attribute::attrubuteFormat(subVal),
                    .offset = static_cast<uint32_t>(
                        reinterpret_cast<int8_t*>(&subVal) - reinterpret_cast<int8_t*>(&val)),
                });
            });

            return *this;
        }

        const auto& attributes() const { return m_attributes; }

        auto& attributes() { return m_attributes; }

        const auto& bindings() const { return m_bindings; }

        auto& bindings() { return m_bindings; }

    private:
        std::vector<Attribute> m_attributes;
        std::vector<Binding> m_bindings;

        CREATE_INFO_PROPERTY(PrimitiveTopology, primitiveTopology, PrimitiveTopology::TRIANGLES);
        CREATE_INFO_PROPERTY(SampleShading, sampleShading, SampleShading::SS_0_PERCENT);
        CREATE_INFO_PROPERTY(CullMode, cullMode, CullMode::BACK);
        CREATE_INFO_PROPERTY(PolygonMode, polygonMode, PolygonMode::FILL);
        CREATE_INFO_PROPERTY(FrontFace, frontFace, FrontFace::COUNTER_CLOCKWISE);
    };
};
