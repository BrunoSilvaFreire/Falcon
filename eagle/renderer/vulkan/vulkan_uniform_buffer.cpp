//
// Created by Novak on 09/06/2019.
//

#include <algorithm>
#include "vulkan_uniform_buffer.h"

namespace eagle {

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data) :
        UniformBuffer(size), m_info(createInfo) {

    if (data != nullptr){
        memcpy(m_bytes.data(), data, size);
    }else{
        memset(m_bytes.data(), 0, size);
    }

    create_uniform_buffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    cleanup();
}

void
VulkanUniformBuffer::flush(uint32_t bufferIndex) {

    if (!is_dirty()) {
        return;
    }
    m_buffers[bufferIndex]->copy_to(m_bytes.data(), m_bytes.size());
    m_dirtyBuffers.erase(bufferIndex);
}

void VulkanUniformBuffer::create_uniform_buffer() {

    if (!m_cleared) return;

    m_buffers.resize(m_info.bufferCount);

    VulkanBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i = 0; i < m_info.bufferCount; i++) {
        VulkanBuffer::create_buffer(m_info.physicalDevice, m_info.device, m_buffers[i],
                                    bufferCreateInfo, m_bytes.size(), m_bytes.data());
        m_buffers[i]->map(m_bytes.size());
    }
    m_cleared = false;
}

void VulkanUniformBuffer::cleanup() {
    if (m_cleared) return;
    for (auto& buffer : m_buffers) {
        buffer->unmap();
        buffer->destroy();
    }
    m_dirtyBuffers.clear();
    m_cleared = true;
}

void VulkanUniformBuffer::upload() {
    m_dirtyBuffers.clear();
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}

bool VulkanUniformBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanUniformBuffer::copy_from(void *data, size_t size, size_t offset) {
    assert(size + offset <= m_bytes.size());
    memcpy(m_bytes.data() + offset, data, size);
}

DescriptorType VulkanUniformBuffer::type() const {
    return DescriptorType::UNIFORM_BUFFER;
}

void VulkanUniformBuffer::recreate(uint32_t bufferCount) {
    m_info.bufferCount = bufferCount;
    create_uniform_buffer();
}


}
