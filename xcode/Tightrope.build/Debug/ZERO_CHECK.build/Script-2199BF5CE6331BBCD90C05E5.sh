#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/adam/Dev/Apophenia/xcode
  make -f /Users/adam/Dev/Apophenia/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/adam/Dev/Apophenia/xcode
  make -f /Users/adam/Dev/Apophenia/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/adam/Dev/Apophenia/xcode
  make -f /Users/adam/Dev/Apophenia/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/adam/Dev/Apophenia/xcode
  make -f /Users/adam/Dev/Apophenia/xcode/CMakeScripts/ReRunCMake.make
fi

