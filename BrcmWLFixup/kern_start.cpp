//
//  kern_start.cpp
//  BrcmWLFixup
//
//  Copyright © 2017 Vanilla. All rights reserved.
//

#include <Headers/plugin_start.hpp>

#include "kern_brcm.hpp"

static BRCM brcm;

// kernel versioning
static KernelVersion kernMajorVersion;      // from kern_brcm.hpp
static KernelMinorVersion kernMinorVersion;
static int sysMajorVersion;
static int sysMinorVersion;

static void setSystemVersions() {
  kernMajorVersion = getKernelVersion();
  sysMajorVersion = kernMajorVersion - 4;
  
  kernMinorVersion = getKernelMinorVersion();
  sysMinorVersion = kernMinorVersion;
}

static void brcmStart() {
  setSystemVersions();
  
  SYSLOG("brcm @ starting on macOS 10.%d.%d", sysMajorVersion, sysMinorVersion);
  brcm.init();
}

static const char *bootargOff[] {
  "-brcmoff"
};

static const char *bootargDebug[] {
  "-brcmdbg"
};

static const char *bootargBeta[] {
  "-brcmbeta"
};

PluginConfiguration ADDPR(config)
{
  xStringify(PRODUCT_NAME),
  
  // Lilu 1.1.0 and greater compatibility
  parseModuleVersion(xStringify(MODULE_VERSION)),
  
  bootargOff,
  arrsize(bootargOff),
  
  bootargDebug,
  arrsize(bootargDebug),
  
  bootargBeta,
  arrsize(bootargBeta),
  
  // minKernel - 10.13
  KernelVersion::HighSierra,
  // maxKernel - 10.13
  KernelVersion::HighSierra,
  
  brcmStart
};
