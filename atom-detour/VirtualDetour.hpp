#pragma once

#include <atom-detour/DetourBase.hpp>

namespace {
  typedef unsigned int uint;
}

namespace atom {
  class VirtualDetour : DetourBase {
  private:
    // Private members
    void** mVirtualTable;
    uint mVirtualTableIndex;

  public:
    /// <summary>
    /// Constructs a virtual detour from an object
    /// </summary>
    VirtualDetour(void* object, uint vtableIndex);

    /// <summary>
    /// Constructs a virtual detour from a virtual table and index
    /// </summary>
    VirtualDetour(void** virtualTable, uint vtableIndex);

    /// <summary>
    /// Destructs the virtual detour and removes the hook
    /// </summary>
    ~VirtualDetour();

    /// <summary>
    /// Returns a callable address to the target function
    /// </summary>
    virtual void* GetCallableAddress() const;

    /// <summary>
    /// Applies the detour to target function
    /// </summary>
    virtual void ApplyHook(void* callback);

    /// <summary>
    /// Removes the detour (if it's applied)
    /// </summary>
    virtual void RemoveHook();
  };

  inline void* VirtualDetour::GetCallableAddress() const {
    return mTarget;
  }
}
