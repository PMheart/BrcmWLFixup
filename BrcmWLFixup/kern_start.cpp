//
//  kern_start.cpp
//  BrcmWLFixup
//
//  Copyright © 2017 Vanilla. All rights reserved.
//

#include <Headers/plugin_start.hpp>

#include "kern_brcm.hpp"

static BRCM brcm;

static const char *bootargOff[] {
  "-brcmoff"
};

static const char *bootargDebug[] {
  "-brcmdbg"
};

static const char *bootargBeta[] {
  "-brcmoff"
};

PluginConfiguration ADDPR(config)
{
  xStringify(PRODUCT_NAME),
  
  // Lilu 1.1.0 and greater compatibility
  parseModuleVersion(xStringify(MODULE_VERSION)),
  
  bootargOff,
  sizeof(bootargOff)/sizeof(bootargOff[0]),
  
  bootargDebug,
  sizeof(bootargDebug)/sizeof(bootargDebug[0]),
  
  bootargBeta,
  sizeof(bootargBeta)/sizeof(bootargBeta[0]),
  
  // minKernel - 10.8
  KernelVersion::MountainLion,
  // maxKernel - 10.13
  KernelVersion::HighSierra,
  
  []() {
    brcm.init();
  }
};
