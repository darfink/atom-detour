#pragma once

#include <atom-detour/DetourBase.hpp>
#include <memory>

namespace {
  typedef unsigned char byte;
  typedef unsigned int uint;
}

namespace atom {
  class StaticDetour : DetourBase {
  private:
    // Private members
    std::shared_ptr<byte> mTrampoline;
    size_t mBytesDisassembled;

    // Static members
    static const byte PatchRelative[];

  public:
    /// <summary>
    /// Constructs a static detour from an object
    /// </summary>
    StaticDetour(void* address);

    /// <summary>
    /// Destructs the static detour and removes the hook
    /// </summary>
    ~StaticDetour();

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

  inline void* StaticDetour::GetCallableAddress() const {
    if(mDetoured == true) {
      return mTrampoline.get();
    } else /* We can just return the original */ {
      return mTarget;
    }
  }
}
