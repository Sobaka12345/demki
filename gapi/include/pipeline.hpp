#ifndef GAPI_PIPELINE_HPP
#define GAPI_PIPELINE_HPP

#include "../vk/pipeline.hpp"

namespace gapi {


template <typename T>
concept SIPipeline = requires(
    T& pipeline,
    const T& constPipeline) {
    { pipeline };
};

template <typename GApiT>
    requires SIPipeline<__private::Pipeline<GApiT>>
struct Pipeline : __private::Pipeline<GApiT>
{};

}

#endif // GAPI_PIPELINE_HPP