#pragma once

#include <mutex>
#include <vector>

#include "dxvk_buffer.h"
#include "dxvk_gpu_query.h"

namespace dxvk {

  class DxvkDevice;
  class DxvkPredicatePool;

  /**
   * \brief Predicate
   * 
   * A predicate stores a small buffer slice
   * that can be used for predication. Once
   * a predicate object runs out of scope,
   * the buffer slice will be recycled.
   */
  class DxvkPredicate : public DxvkResource {

  public:

    DxvkPredicate(
            DxvkPredicatePool*  pool,
            DxvkBufferSlice     slice);

    ~DxvkPredicate();

    DxvkBufferSlice slice() const {
      return m_slice;
    }

    DxvkPhysicalBufferSlice physicalSlice() const {
      return m_slice.physicalSlice();
    }

    DxvkDescriptorInfo getDescriptor() const {
      return m_slice.getDescriptor();      
    }

  private:

    DxvkPredicatePool*  m_pool;
    DxvkBufferSlice     m_slice;

  };

  
  /**
   * \brief Predicate pool
   * 
   * Provides a simple buffer allocator
   * for predicate objects.
   */
  class DxvkPredicatePool : public RcObject {
    friend class DxvkPredicate;
  public:

    DxvkPredicatePool(DxvkDevice* device);
    ~DxvkPredicatePool();

    /**
     * \brief Creates a predicate
     * 
     * Re-uses an existing buffer slice for the
     * predicate object or creates a new one.
     * \returns New predicate object
     */
    Rc<DxvkPredicate> createPredicate();

  private:

    DxvkDevice*                   m_device;

    std::mutex                    m_mutex;
    std::vector<DxvkBufferSlice>  m_slices;

    void freeSlice(const DxvkBufferSlice& slice);

    void createPredicateBuffer();

  };

}