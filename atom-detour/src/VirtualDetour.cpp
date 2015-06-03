#include <atom-memory/MemoryRegion.hpp>
#include <atom-detour/VirtualDetour.hpp>

namespace atom {
  VirtualDetour::VirtualDetour(void* object, uint vtableIndex) :
      VirtualDetour(*reinterpret_cast<void***>(object), vtableIndex) {
  }

  VirtualDetour::VirtualDetour(void** virtualTable, uint vtableIndex) :
      DetourBase(virtualTable[vtableIndex]),
      mVirtualTable(virtualTable),
      mVirtualTableIndex(vtableIndex) {
  }

  VirtualDetour::~VirtualDetour() {
    this->RemoveHook();
  }

  void VirtualDetour::ApplyHook(void* callback) {
    if(mDetoured) {
      return;
    }

    MemoryRegion region(mVirtualTable[mVirtualTableIndex], sizeof(void*));
    region.ExecuteFunction(MemoryRegion::ReadWriteExecute, [this, callback]() {
      mVirtualTable[mVirtualTableIndex] = callback;
      mDetoured = true;
    });
  }

  void VirtualDetour::RemoveHook() {
    if(!mDetoured) {
      return;
    }

    MemoryRegion region(mVirtualTable[mVirtualTableIndex], sizeof(void*));
    region.ExecuteFunction(MemoryRegion::ReadWriteExecute, [this]() {
      mVirtualTable[mVirtualTableIndex] = mTarget;
      mDetoured = false;
    });
  }
}
