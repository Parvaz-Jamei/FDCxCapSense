#pragma once

#ifndef FDCX_ENABLE_PROFILES
#define FDCX_ENABLE_PROFILES 1
#endif
#ifndef FDCX_ENABLE_EXPERIMENTAL_PROFILES
#define FDCX_ENABLE_EXPERIMENTAL_PROFILES 1
#endif

#include "FDCxCapSense_FDC1004.h"
#include "FDCxCapSense_FDC2x1x.h"
#include "fdcx/FDCxDiagnostics.h"
#include "fdcx/FDCxProfile.h"

#if FDCX_ENABLE_PROFILES
#include "processing/Hysteresis.h"
#include "processing/DerivativeDetector.h"
#include "processing/Filters.h"
#include "profiles/ProfileInput.h"
#include "fdcx/profiles/FDCxProfileTypes.h"
#include "fdcx/profiles/FDCxProfileResult.h"
#include "fdcx/profiles/FDCxThreshold.h"
#include "fdcx/profiles/FDCxDebounce.h"
#include "fdcx/profiles/FDCxConfidence.h"
#endif

#if FDCX_ENABLE_PROFILES && FDCX_ENABLE_EXPERIMENTAL_PROFILES
#include "profiles/LiquidLevelProfile.h"
#include "profiles/ProximityProfile.h"
#include "profiles/SurfaceStateProfile.h"
#include "profiles/MaterialProfile.h"
#include "profiles/PowderGranularProfile.h"
#include "profiles/AutomotiveProximityDemo.h"
#endif

#define FDCXCAPSENSE_VERSION "0.6.11"
#define FDCXCAPSENSE_PHASE3_EXPERIMENTAL_ALPHA 1
#define FDCXCAPSENSE_ROADMAP_V6_ALIGNED 1
#define FDCXCAPSENSE_PUBLIC_RELEASE_CANDIDATE 0
#define FDCXCAPSENSE_PHASE4_FDC2X1X_FREQUENCY_FIRST_RC 1
#define FDCXCAPSENSE_FDC2X1X_REAL_HARDWARE_STARTUP_PATCH 1
#define FDCXCAPSENSE_FDC2X1X_CONFIG_REGISTER_FIX 1

#define FDCXCAPSENSE_INDUSTRIAL_RELEASE_HARDENING 1
