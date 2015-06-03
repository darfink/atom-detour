#pragma once

#include <cassert>

namespace atom {
  class DetourBase {
  protected:
    // Protected members
    bool mDetoured;
    void* mTarget;

  public:
    /// <summary>
    /// Destructs the detour object
    /// </summary>
    virtual ~DetourBase();

    /// <summary>
    /// Returns a callable address to the target function
    /// </summary>
    virtual void* GetCallableAddress() const = 0;

    /// <summary>
    /// Applies the detour to target function
    /// </summary>
    virtual void ApplyHook(void* callback) = 0;

    /// <summary>
    /// Removes the detour (if it's applied)
    /// </summary>
    virtual void RemoveHook() = 0;

    /// <summary>
    /// Returns whether the function is detoured or not
    /// </summary>
    bool IsDetoured() const;

  protected:
    /// <summary>
    /// Constructs a detour base object
    /// </summary>
    DetourBase(void* address);
  };

  inline DetourBase::~DetourBase() {
  }

  inline DetourBase::DetourBase(void* address) :
      mDetoured(false),
      mTarget(address) {
    assert(address != nullptr);
  }

  inline bool DetourBase::IsDetoured() const {
    return mDetoured;
  }
}
