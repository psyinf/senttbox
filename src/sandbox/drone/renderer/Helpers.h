#pragma once
#include <components/OrbitalParameters.h>
#include <gmtl/gmtl.h>
#include <vsg/all.h>

template <class DATA_TYPE>
vsg::t_vec3<DATA_TYPE> gmtlToVsg(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::t_vec3(v[0], v[1], v[2]);
}

template <class DATA_TYPE>
auto gmtlToVsgd(const gmtl::Vec<DATA_TYPE, 3>& v)
{
    return vsg::dvec3(static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]));
}
vsg::ref_ptr<vsg::Node> createTexturedQuad(vsg::ref_ptr<const vsg::Options> options)
{
    // load shaders
    vsg::ref_ptr<vsg::ShaderStage> vertexShader   = vsg::ShaderStage::read(VK_SHADER_STAGE_VERTEX_BIT, "main", vsg::findFile("data/shaders/Custom.vert", options));
    vsg::ref_ptr<vsg::ShaderStage> fragmentShader = vsg::ShaderStage::read(VK_SHADER_STAGE_FRAGMENT_BIT, "main", vsg::findFile("data/shaders/Custom.frag", options));
    if (!vertexShader || !fragmentShader)
    {
        std::cout << "Could not create shaders." << std::endl;
        return {};
    }

    // read texture image
    vsg::Path textureFile("data/textures/lz.vsgb");
    auto      textureData = vsg::read_cast<vsg::Data>(textureFile, options);
    if (!textureData)
    {
        std::cout << "Could not read texture file : " << textureFile << std::endl;
        return {};
    }

    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr} // { binding, descriptorTpe, descriptorCount, stageFlags, pImmutableSamplers}
    };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create(descriptorBindings);

    vsg::PushConstantRanges pushConstantRanges{
        {VK_SHADER_STAGE_VERTEX_BIT, 0, 128} // projection view, and model matrices, actual push constant calls automatically provided by the VSG's DispatchTraversal
    };

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{
        VkVertexInputBindingDescription{0, sizeof(vsg::vec3), VK_VERTEX_INPUT_RATE_VERTEX}, // vertex data
        VkVertexInputBindingDescription{1, sizeof(vsg::vec3), VK_VERTEX_INPUT_RATE_VERTEX}, // colour data
        VkVertexInputBindingDescription{2, sizeof(vsg::vec2), VK_VERTEX_INPUT_RATE_VERTEX}  // tex coord data
    };

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{
        VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}, // vertex data
        VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0}, // colour data
        VkVertexInputAttributeDescription{2, 2, VK_FORMAT_R32G32_SFLOAT, 0},    // tex coord data
    };

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create(vertexBindingsDescriptions, vertexAttributeDescriptions),
        vsg::InputAssemblyState::create(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false),
        vsg::RasterizationState::create(),
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create()};

    auto pipelineLayout       = vsg::PipelineLayout::create(vsg::DescriptorSetLayouts{descriptorSetLayout}, pushConstantRanges);
    auto graphicsPipeline     = vsg::GraphicsPipeline::create(pipelineLayout, vsg::ShaderStages{vertexShader, fragmentShader}, pipelineStates);
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create(graphicsPipeline);

    // create texture image and associated DescriptorSets and binding
    auto texture = vsg::DescriptorImage::create(vsg::Sampler::create(), textureData, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    auto descriptorSet      = vsg::DescriptorSet::create(descriptorSetLayout, vsg::Descriptors{texture});
    auto bindDescriptorSets = vsg::BindDescriptorSets::create(VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->layout, 0, vsg::DescriptorSets{descriptorSet});

    // create StateGroup as the root of the scene/command graph to hold the GraphicsProgram, and binding of Descriptors to decorate the whole graph
    auto scenegraph = vsg::StateGroup::create();
    scenegraph->add(bindGraphicsPipeline);
    scenegraph->add(bindDescriptorSets);

    // set up model transformation node
    auto transform = vsg::MatrixTransform::create(); // VK_SHADER_STAGE_VERTEX_BIT

    // add transform to root of the scene graph
    scenegraph->addChild(transform);

    // set up vertex and index arrays
    auto vertices = vsg::vec3Array::create(
        {{-0.5f, -0.5f, 0.0f},
         {0.5f, -0.5f, 0.05f},
         {0.5f, 0.5f, 0.0f},
         {-0.5f, 0.5f, 0.0f},
         {-0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f, -0.5f},
         {0.5f, 0.5f, -0.5},
         {-0.5f, 0.5f, -0.5}}); // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_INSTANCE, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    auto colors = vsg::vec3Array::create(
        {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
        }); // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    auto texcoords = vsg::vec2Array::create(
        {{0.0f, 0.0f},
         {1.0f, 0.0f},
         {1.0f, 1.0f},
         {0.0f, 1.0f},
         {0.0f, 0.0f},
         {1.0f, 0.0f},
         {1.0f, 1.0f},
         {0.0f, 1.0f}}); // VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    auto indices = vsg::ushortArray::create(
        {0, 1, 2,
         2, 3, 0,
         4, 5, 6,
         6, 7, 4}); // VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    // setup geometry
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(0, vsg::DataList{vertices, colors, texcoords}));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(12, 1, 0, 0, 0));

    // add drawCommands to transform
    transform->addChild(drawCommands);

    return scenegraph;
}

auto linePipeline(vsg::ref_ptr<const vsg::Options> options)
{
    // load shaders
    vsg::ref_ptr<vsg::ShaderStage> vertexShader   = vsg::ShaderStage::read(VK_SHADER_STAGE_VERTEX_BIT, "main", vsg::findFile("data/shaders/Custom.vert", options));
    vsg::ref_ptr<vsg::ShaderStage> fragmentShader = vsg::ShaderStage::read(VK_SHADER_STAGE_FRAGMENT_BIT, "main", vsg::findFile("data/shaders/Custom.frag", options));
    if (!vertexShader || !fragmentShader)
    {
        std::cout << "Could not create shaders." << std::endl;
        throw std::runtime_error{"Could not create shaders."};
    }
    // set up graphics pipeline
    vsg::DescriptorSetLayoutBindings descriptorBindings{
        {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr} // { binding, descriptorTpe, descriptorCount, stageFlags, pImmutableSamplers}
    };

    auto descriptorSetLayout = vsg::DescriptorSetLayout::create(descriptorBindings);

    vsg::PushConstantRanges pushConstantRanges{
        {VK_SHADER_STAGE_VERTEX_BIT, 0, 128} // projection view, and model matrices, actual push constant calls automatically provided by the VSG's DispatchTraversal
    };

    vsg::VertexInputState::Bindings vertexBindingsDescriptions{
        VkVertexInputBindingDescription{0, sizeof(vsg::vec3), VK_VERTEX_INPUT_RATE_VERTEX},  // vertex data
        VkVertexInputBindingDescription{1, sizeof(vsg::vec3), VK_VERTEX_INPUT_RATE_INSTANCE} // colour data
    };

    vsg::VertexInputState::Attributes vertexAttributeDescriptions{
        VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}, // vertex data
        VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0}  // colour data
    };

    vsg::GraphicsPipelineStates pipelineStates{
        vsg::VertexInputState::create(vertexBindingsDescriptions, vertexAttributeDescriptions),
        vsg::InputAssemblyState::create(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false),
        vsg::RasterizationState::create(),
        vsg::MultisampleState::create(),
        vsg::ColorBlendState::create(),
        vsg::DepthStencilState::create()};

    auto pipelineLayout       = vsg::PipelineLayout::create(vsg::DescriptorSetLayouts{descriptorSetLayout}, pushConstantRanges);
    auto graphicsPipeline     = vsg::GraphicsPipeline::create(pipelineLayout, vsg::ShaderStages{vertexShader, fragmentShader}, pipelineStates);
    auto bindGraphicsPipeline = vsg::BindGraphicsPipeline::create(graphicsPipeline);


    // create StateGroup as the root of the scene/command graph to hold the GraphicsProgram, and binding of Descriptors to decorate the whole graph
    auto scenegraph = vsg::StateGroup::create();
    scenegraph->add(bindGraphicsPipeline);
    return scenegraph;
}
vsg::ref_ptr<vsg::Node> createOrbit(const OrbitalParameters& op, vsg::ref_ptr<const vsg::Options> options)
{
    //FIXME: currently we are creating a new pipeline for each orbit. 
    auto scenegraph = linePipeline(options);
    // set up model transformation node
    auto transform = vsg::MatrixTransform::create(); // VK_SHADER_STAGE_VERTEX_BIT

    // add transform to root of the scene graph
    scenegraph->addChild(transform);
    auto num_steps = 360;
    auto vertices = vsg::vec3Array::create(num_steps +1);
    for (int i = 0; i <= num_steps; i++)
    {
        // get the position of the orbiting object
        vertices->at(i) = gmtlToVsgd(OrbitalMechanics::getEulerAnglesFromEccentricAnomaly(op, vsg::PI * 2.0 * i / num_steps).toCartesian());
    }
    // set up vertex and index arrays
    auto colors = vsg::vec3Array::create(vertices->size());
       
    auto indices = vsg::ushortArray::create(vertices->size());
    for (auto i = 0; i < vertices->size(); i++)
    {
        indices->at(i) = i;
        colors->at(i)  = {1.0f, 0.0f, 1.0f};
    }


    // setup geometry
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(0, vsg::DataList{vertices, colors}));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(indices->size(), 1, 0, 0, 0));

    // add drawCommands to transform
    transform->addChild(drawCommands);

    return scenegraph;
}

vsg::ref_ptr<vsg::Node> createLines(vsg::ref_ptr<const vsg::Options> options)
{

    static auto scenegraph = linePipeline(options);
    // set up model transformation node
    auto transform = vsg::MatrixTransform::create(); // VK_SHADER_STAGE_VERTEX_BIT

    // add transform to root of the scene graph
    scenegraph->addChild(transform);

    // set up vertex and index arrays
    auto vertices = vsg::vec3Array::create(
        {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
        }); // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_INSTANCE, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    auto colors = vsg::vec3Array::create(
        {{1.0f, 0.0f, 1.0f}}); // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    auto indices = vsg::ushortArray::create(
        {0, 1}); // VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE

    // setup geometry
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(0, vsg::DataList{vertices, colors}));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(2, 1, 0, 0, 0));

    // add drawCommands to transform
    transform->addChild(drawCommands);

    return scenegraph;
}
