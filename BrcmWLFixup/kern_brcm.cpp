//
//  kern_brcm.cpp
//  BrcmWLFixup
//
//  Copyright © 2017 Vanilla. All rights reserved.
//

#include <Headers/kern_api.hpp>

#include "kern_brcm.hpp"

// Only used in apple-driven callbacks
static BRCM          *callbackBoardID = nullptr;
static KernelPatcher *callbackPatcher = nullptr;

static const char *kextAirPortBrcm4360[] {
  "/System/Library/Extensions/IO80211Family.kext/Contents/PlugIns/AirPortBrcm4360.kext/Contents/MacOS/AirPortBrcm4360"
};

static const char *kextAirPortBrcmNIC[] {
  "/System/Library/Extensions/IO80211Family.kext/Contents/PlugIns/AirPortBrcmNIC.kext/Contents/MacOS/AirPortBrcmNIC"
};

static const char *kextAirPortBrcmNIC_MFG[] {
  "/System/Library/Extensions/AirPortBrcmNIC-MFG.kext/Contents/MacOS/AirPortBrcmNIC-MFG"
};

static KernelPatcher::KextInfo kextList[] {
  { "com.apple.driver.AirPort.Brcm4360", kextAirPortBrcm4360, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
  { "com.apple.driver.AirPort.BrcmNIC", kextAirPortBrcmNIC, 1, true, {}, KernelPatcher::KextInfo::Unloaded },
  { "com.apple.driver.AirPort.BrcmNIC-MFG", kextAirPortBrcmNIC_MFG, 1, true, {}, KernelPatcher::KextInfo::Unloaded }
};

static size_t kextListSize {3};

bool BRCM::init() {
  LiluAPI::Error error = lilu.onKextLoad(kextList, kextListSize,
                                         [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    callbackBoardID = static_cast<BRCM *>(user);
    callbackPatcher = &patcher;
    callbackBoardID->processKext(patcher, index, address, size);
  }, this);
  
  if (error != LiluAPI::Error::NoError) {
    SYSLOG("brcm @ failed to register onPatcherLoad %d", error);
    return false;
  }
  
  return true;
}

bool BRCM::checkBoardId(const char *boardID) {
  DBGLOG("brcm @ forcing brcm driver for board %s", boardID);
  
  // just let it return true, that's all we want
  return true;
}

void BRCM::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
  if (!(progressState & ProcessingState::AnythingDone)) {
    for (size_t i = 0; i < kextListSize; i++) {
      if (kextList[i].loadIndex == index) {
        // patch AirPortBrcm4360
        if (!strcmp(kextList[i].id, "com.apple.driver.AirPort.Brcm4360")) {
          DBGLOG("brcm @ found com.apple.driver.AirPort.Brcm4360");
          auto check_Board_Id = patcher.solveSymbol(index, "__ZN16AirPort_Brcm436012checkBoardIdEPKc");
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained __ZN16AirPort_Brcm436012checkBoardIdEPKc");
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(checkBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed __ZN16AirPort_Brcm436012checkBoardIdEPKc");
              progressState |= ProcessingState::BRCM4360Patched;
              break;
            }
          }
        }
        
        // patch AirPortBrcmNIC
        if (!strcmp(kextList[i].id, "com.apple.driver.AirPort.BrcmNIC")) {
          DBGLOG("brcm @ found com.apple.driver.AirPort.BrcmNIC");
          auto check_Board_Id = patcher.solveSymbol(index, "__ZN15AirPort_BrcmNIC12checkBoardIdEPKc");
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained __ZN15AirPort_BrcmNIC12checkBoardIdEPKc");
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(checkBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed __ZN15AirPort_BrcmNIC12checkBoardIdEPKc");
              progressState |= ProcessingState::BRCMNICPatched;
              break;
            }
          }
        }
        
        // patch AirPortBrcmNIC-MFG
        if (!strcmp(kextList[i].id, "com.apple.driver.AirPort.BrcmNIC-MFG")) {
          DBGLOG("brcm @ found com.apple.driver.AirPort.BrcmNIC-MFG");
          auto check_Board_Id = patcher.solveSymbol(index, "__ZN19AirPort_BrcmNIC_MFG12checkBoardIdEPKc");
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained __ZN19AirPort_BrcmNIC_MFG12checkBoardIdEPKc");
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(checkBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed __ZN19AirPort_BrcmNIC_MFG12checkBoardIdEPKc");
              progressState |= ProcessingState::BRCMNIC_MFGPatched;
              break;
            }
          }
        }
      }
      // Ignore all the errors for other processors
      patcher.clearError();
    }
  }
}
