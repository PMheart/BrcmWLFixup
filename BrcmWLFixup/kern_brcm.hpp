//
//  kern_brcm.hpp
//  BrcmWLFixup
//
//  Copyright © 2017 Vanilla. All rights reserved.
//

#ifndef kern_brcm_hpp
#define kern_brcm_hpp

#include <Headers/kern_patcher.hpp>

// get array size, from syscl's repo
// https://github.com/syscl/EnableLidWake/blob/master/EanbleLidWake/syscl_lib.h#L41
#define getArrayLength(Array) ((sizeof(Array)/sizeof(0[Array])) / ((size_t)(!(sizeof(Array) % sizeof(0[Array])))))

class BRCM {
public:
  bool init();
  
private:  
  /**
   *  Patch kext if needed and prepare other patches
   *
   *  @param patcher KernelPatcher instance
   *  @param index   kinfo handle
   *  @param address kinfo load address
   *  @param size    kinfo memory size
   */
  void processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
  
  /**
   *  board-id check func type
   */
  using t_check_Board_Id = bool (*)(const char *);
  
  /**
   *  Hooked funcs
   */
  static bool myCheckBoardId(const char *boardID);
  
  /**
   *  Trampolines for original method invocations
   */
  t_check_Board_Id orgCheckBoardId = nullptr;
  
  struct ProcessingState {
    enum {
      NothingReady       = 0,
      BRCM4360Patched    = 1,
      BRCMNICPatched     = 2,
      BRCMNIC_MFGPatched = 4,
      AnythingDone       = BRCM4360Patched | BRCMNICPatched | BRCMNIC_MFGPatched
    };
  };
  
  int progressState {ProcessingState::NothingReady};
};

#endif /* kern_brcm_hpp */
