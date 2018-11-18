#include "dxvk_device.h"

namespace dxvk {

  DxvkPredicate::DxvkPredicate(
          DxvkPredicatePool*  pool,
          DxvkBufferSlice     slice)
  : m_pool(pool), m_slice(slice) {

  }


  DxvkPredicate::~DxvkPredicate() {
    m_pool->freeSlice(m_slice);
  }




  DxvkPredicatePool::DxvkPredicatePool(DxvkDevice* device)
  : m_device(device) {

  }


  DxvkPredicatePool::~DxvkPredicatePool() {

  }


  Rc<DxvkPredicate> DxvkPredicatePool::createPredicate() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_slices.size() == 0)
      createPredicateBuffer();
    
    DxvkBufferSlice slice = m_slices.back();
    m_slices.pop_back();

    return new DxvkPredicate(this, std::move(slice));
  }


  void DxvkPredicatePool::freeSlice(const DxvkBufferSlice& slice) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_slices.push_back(slice);
  }


  void DxvkPredicatePool::createPredicateBuffer() {
    const uint32_t sliceCount = 1024;
    const uint32_t sliceAlign = 4;

    DxvkBufferCreateInfo info;
    info.size   = 1024 * sizeof(uint32_t);
    info.usage  = VK_BUFFER_USAGE_TRANSFER_DST_BIT
                | VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT;
    info.stages = VK_PIPELINE_STAGE_TRANSFER_BIT
                | VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
    info.access = VK_ACCESS_TRANSFER_WRITE_BIT
                | VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT;
    
    Rc<DxvkBuffer> buffer = m_device->createBuffer(
      info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    for (uint32_t i = 0; i < sliceCount; i++) {
      m_slices.push_back(DxvkBufferSlice(buffer,
        i * sliceAlign, sizeof(uint32_t)));
    }
  }

}