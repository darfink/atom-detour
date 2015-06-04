#include <atom-detour/StaticDetour.hpp>
#include <atom-memory/MemoryRegion.hpp>
#include <atom-memory/Memory.hpp>
#include <udis86.h>

// Used to get the size of arrays with a static size
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

namespace atom {
  // Prevent tons of typing
  typedef unsigned char byte;
  typedef unsigned int uint;

  // 'jmp <relative>' - This is probably the best detour type to use
  const byte StaticDetour::PatchRelative[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

  StaticDetour::StaticDetour(void* address) :
      DetourBase(address) {
    ud_t ud;
    ud_init(&ud);

    // Assume the address consists of at least 20 bytes
    // TODO: Try to find a better solution than this (e.g search for 'ret'?)
    ud_set_input_buffer(&ud, static_cast<byte*>(mTarget), 20);
    ud_set_mode(&ud, 32);

    while(mBytesDisassembled < ARRAY_SIZE(PatchRelative)) {
      size_t bytes = ud_disassemble(&ud);

      if(bytes == 0) {
          throw Exception(ATOM_EXCEPTION_INFO, "Couldn't disassemble enough bytes from target address");
      } else {
          mBytesDisassembled += bytes;
      }
    }
  }

  void StaticDetour::ApplyHook(void* callback) {
    if(mDetoured) {
      return;
    }
    
    size_t allocated;
    void* memory = Memory::Allocate(
      mBytesDisassembled + ARRAY_SIZE(PatchRelative),
      Memory::ReadWriteExecute,
      &allocated);

    // Allocate executable memory to backup the original function (i.e the trampoline)
    mTrampoline.reset(static_cast<byte*>(memory), [allocated](byte* memory) {
      Memory::Free(memory, allocated);
    });

    MemoryRegion region(mTarget, mBytesDisassembled);
    region.ExecuteFunction(Memory::ReadWriteExecute, [this, callback]() {
      // Copy the original function bytes to our trampoline
      std::memcpy(mTrampoline.get(), mTarget, mBytesDisassembled);

      // To avoid any execution of the function whilst it is being modified, we
      // first create the patch in this vector, so we can copy it in one sweep
      std::vector<byte> patch(ARRAY_SIZE(PatchRelative));
      std::memcpy(patch.data(), PatchRelative, patch.size());

      // Calculate the relative address to the callback function from the current EIP
      *reinterpret_cast<uint*>(patch.data() + 0x01) =
        (static_cast<byte*>(callback) - static_cast<byte*>(mTarget)) - patch.size();
  
      // Copy the patch to the original function to detour it!
      std::memcpy(mTarget, patch.data(), patch.size());

      uint delta = mBytesDisassembled - ARRAY_SIZE(PatchRelative);

      if(delta > 0) {
        std::vector<byte> nops(delta);
        std::fill(nops.begin(), nops.end(), 0x90);

        // In case we disassembled more instructions than we've replaced, there might be
        // corrupt ones left in the function. These will never be executed but just because
        // they _might_ be, we replace those invalid instructions with normal 'nops'.
        std::memcpy(static_cast<byte*>(mTarget) + ARRAY_SIZE(PatchRelative), nops.data(), nops.size());
      }

      // If the user wants to execute the original function, we must first execute the bytes
      // that we replaced with the detour, and then jump to the rest of the function. So we
      // do this by adding a relative jump at the end of our trampoline.
      std::memcpy(patch.data(), PatchRelative, patch.size());

      // Calculate the relative address to the callback function (the user provided one) from the current EIP
      *reinterpret_cast<uint*>(patch.data() + 0x01) =
        (static_cast<byte*>(mTarget) - static_cast<byte*>(&mTrampoline.get()[mBytesDisassembled])) - patch.size() + mBytesDisassembled;
      std::memcpy(&mTrampoline.get()[mBytesDisassembled], patch.data(), patch.size());

      // We have successfully hooked the function!
      mDetoured = true;
    });
  }

  void StaticDetour::RemoveHook() {
    if(!mDetoured) {
      return;
    }

    MemoryRegion region(mTarget, mBytesDisassembled);
    region.ExecuteFunction(Memory::ReadWriteExecute, [this]() {
      // Simply copy the previously disassembled bytes
      std::memcpy(mTarget, mTrampoline.get(), mBytesDisassembled);

      // Free our trampoline
      mTrampoline.reset();
      mDetoured = false;
    });
  }
}
