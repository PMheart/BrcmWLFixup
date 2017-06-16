//
//  kern_brcm.cpp
//  BrcmWLFixup
//
//  Copyright © 2017 Vanilla. All rights reserved.
//

#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>

#include "kern_brcm.hpp"

#define kBrcm4360 0
#define kBrcmNIC  1
#define kBrcmMFG  2

// Only used in apple-driven callbacks
static BRCM          *callbackBoardID = nullptr;
static KernelPatcher *callbackPatcher = nullptr;

static const char *idList[] {
  "com.apple.driver.AirPort.Brcm4360",
  "com.apple.driver.AirPort.BrcmNIC",
  "com.apple.driver.AirPort.BrcmNIC-MFG"
};

static const char *binList[] {
  "/System/Library/Extensions/IO80211Family.kext/Contents/PlugIns/AirPortBrcm4360.kext/Contents/MacOS/AirPortBrcm4360",
  "/System/Library/Extensions/IO80211Family.kext/Contents/PlugIns/AirPortBrcmNIC.kext/Contents/MacOS/AirPortBrcmNIC",
  "/System/Library/Extensions/AirPortBrcmNIC-MFG.kext/Contents/MacOS/AirPortBrcmNIC-MFG"
};

static const char *symbolList[] {
  "__ZN16AirPort_Brcm436012checkBoardIdEPKc",
  "__ZN15AirPort_BrcmNIC12checkBoardIdEPKc",
  "__ZN19AirPort_BrcmNIC_MFG12checkBoardIdEPKc"
};

static KernelPatcher::KextInfo kextList[] {
  { idList[kBrcm4360], &binList[kBrcm4360], 1, true, {}, KernelPatcher::KextInfo::Unloaded },
  { idList[kBrcmNIC],  &binList[kBrcmNIC],  1, true, {}, KernelPatcher::KextInfo::Unloaded },
  { idList[kBrcmMFG],  &binList[kBrcmMFG],  1, true, {}, KernelPatcher::KextInfo::Unloaded }
};
static size_t kextListSize = getArrayLength(kextList);


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
  
  if (getKernelVersion() < KernelVersion::HighSierra) {
    SYSLOG("brcm @ loaded on unsupported macOS");
    return false;
  }
  
  return true;
}

bool BRCM::myCheckBoardId(const char *boardID) {
  if (callbackBoardID && callbackPatcher && callbackBoardID->orgCheckBoardId) {
    DBGLOG("brcm @ forcing brcm driver for board %s", boardID);
    // just let it return true, that's all we want
    return true;
  } else {
    SYSLOG("brcm @ checkBoardId callbabck arrived at nowhere");
    return false;
  }
}

void BRCM::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
  if (!(progressState & ProcessingState::AnythingDone)) {
    for (size_t i = 0; i < kextListSize; i++) {
      if (kextList[i].loadIndex == index) {
        // patch AirPortBrcm4360
        // Note: AirPortBrcm4360 is being obsolete under 10.13,
        //       once it gets deprecated, then we'll remove this support
        if (!strcmp(kextList[i].id, idList[kBrcm4360])) {
          DBGLOG("brcm @ found %s", idList[kBrcm4360]);
          auto check_Board_Id = patcher.solveSymbol(index, symbolList[kBrcm4360]);
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained %s", symbolList[kBrcm4360]);
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(myCheckBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed %s", symbolList[kBrcm4360]);
              progressState |= ProcessingState::BRCM4360Patched;
              break;
            } else {
              SYSLOG("brcm @ failed to hook %s callback", symbolList[kBrcm4360]);
            }
          }
        }
        
        // patch AirPortBrcmNIC
        if (!strcmp(kextList[i].id, idList[kBrcmNIC])) {
          DBGLOG("brcm @ found %s", idList[kBrcmNIC]);
          auto check_Board_Id = patcher.solveSymbol(index, symbolList[kBrcmNIC]);
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained %s", symbolList[kBrcmNIC]);
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(myCheckBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed %s", symbolList[kBrcmNIC]);
              progressState |= ProcessingState::BRCMNICPatched;
              break;
            } else {
              SYSLOG("brcm @ failed to hook %s callback", symbolList[kBrcmNIC]);
            }
          }
        }
        
        // patch AirPortBrcmNIC-MFG
        if (!strcmp(kextList[i].id, idList[kBrcmMFG])) {
          DBGLOG("brcm @ found %s", idList[kBrcmMFG]);
          auto check_Board_Id = patcher.solveSymbol(index, symbolList[kBrcmMFG]);
          if (check_Board_Id) {
            DBGLOG("brcm @ obtained %s", symbolList[kBrcmMFG]);
            orgCheckBoardId = reinterpret_cast<t_check_Board_Id>(patcher.routeFunction(check_Board_Id, reinterpret_cast<mach_vm_address_t>(myCheckBoardId), true));
            if (patcher.getError() == KernelPatcher::Error::NoError) {
              DBGLOG("brcm @ routed %s", symbolList[kBrcmMFG]);
              progressState |= ProcessingState::BRCMNIC_MFGPatched;
              break;
            } else {
              SYSLOG("brcm @ failed to hook %s callback", symbolList[kBrcmMFG]);
            }
          }
        }
        
        // new placeholder
      }
      // Ignore all the errors for other processors
      patcher.clearError();
    }
  }
}
