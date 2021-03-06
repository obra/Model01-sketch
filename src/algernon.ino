/* -*- mode: c++ -*-
 * Model01-Sketch -- algernon's Model01 Sketch
 * Copyright (C) 2016, 2017  Gergely Nagy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "00-config.h"

#include <Kaleidoscope.h>
#include <Kaleidoscope-Macros.h>
#include <Kaleidoscope-LEDControl.h>
#include <Kaleidoscope-FingerPainter.h>
#include <Kaleidoscope-Focus.h>

#include "LED-Off.h"

#include <Kaleidoscope-HostOS.h>
#include <Kaleidoscope/HostOS-select.h>
#include <Kaleidoscope-LangPack-Hungarian.h>
#include <Kaleidoscope-LED-ActiveModColor.h>
#include <Kaleidoscope-LED-Stalker.h>
#include <Kaleidoscope-Escape-OneShot.h>
#include <Kaleidoscope-MouseGears.h>
#include <Kaleidoscope-EEPROM-Settings.h>
#include <Kaleidoscope-EEPROM-Keymap.h>
#include <Kaleidoscope-EEPROM-Keymap-Programmer.h>

#include "Layers.h"

#include "Colormap.h"
#include "Focus.h"
#include "Leader.h"
#include "OneShot.h"
#include "TapDance.h"
#include "MagicCombo.h"
#include "Syster.h"
#include "Settings.h"

#include "keymap.h"

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  if (!key_toggled_on (keyState))
    return MACRO_NONE;

  switch (macroIndex) {
  case APPSEL_MUSIC ... APPSEL_WEB:
    Serial.print (F("appsel:"));
    break;
  }

  switch (macroIndex) {
  case APPSEL_MUSIC:
    Serial.println (F("music"));
    break;
  case APPSEL_CHAT:
    Serial.println (F("chat"));
    break;
  case APPSEL_EMACS:
    Serial.println (F("emacs"));
    break;
  case APPSEL_TERM:
    Serial.println (F("term"));
    break;
  case APPSEL_WEB:
    Serial.println (F("web"));
    break;

  case MSP:
    MouseGears.speedUp ();
    break;
  case MSM:
    MouseGears.speedDown ();
    break;
  }
  return MACRO_NONE;
}

static void emptyLayerForceOff (bool postClear) {
  if (postClear)
    return;

  if (!(KeyboardHardware.leftHandState.all & R3C6) &&
      !(KeyboardHardware.rightHandState.all & R3C9)) {
    Layer.off (_EMPTY);
  }
}

static Key getKey (uint8_t layer, byte row, byte col) {
  if (layer != _EMPTY)
    return EEPROMKeymap.getKey (layer, row, col);

  if (row == 3 && (col == 6 || col == 9))
    return Key_Transparent;

  return Key_NoKey;
}

class LEDNone_ : public LEDMode {
public:
  LEDNone_ (void) { };

  virtual void update (void) final {};
};

static LEDNone_ LEDNone;

void setup () {
  Serial.begin(9600);

  Mouse.begin ();
  AbsoluteMouse.begin ();

  Kaleidoscope.setup ();

#if WITH_STALKER_EFFECT
  StalkerEffect.configure (STALKER (BlazingTrail, NULL));
#endif

  loop_hook_use (emptyLayerForceOff);

  Kaleidoscope.use (&LEDNone,
                    &FingerPainter,
#if WITH_STALKER_EFFECT
                    &StalkerEffect,
#endif
                    &HostOS,
#if WITH_KEYMAP_PROGRAMMER
                    &EEPROMKeymapProgrammer,
#endif
                    NULL);

  algernon::Settings::configure ();
  algernon::Colormap::configure ();

  algernon::Leader::configure ();
  algernon::TapDance::configure ();
  algernon::OneShot::configure ();
  algernon::Syster::configure ();
  algernon::MagicCombo::configure ();

  Kaleidoscope.use (&EscapeOneShot,
                    &Macros,
                    &Hungarian,
                    &MouseGears,
                    &ActiveModColorEffect,
                    NULL);

  algernon::FocusSetup::configure ();

  Layer.getKey = getKey;

  LEDControl.syncDelay = 64;
  delay (1000);

  FingerPainter.configure ();
  FingerPainter.defaultPalette = FINGERPAINTER_PALETTE_PASTEL;

  algernon::Settings::seal ();

  Focus.addHook (FOCUS_HOOK_FINGERPAINTER);
}

#if WITH_CYCLE_REPORT
static unsigned long avgLoopTime = 0;
static unsigned long nextReport = millis() + 1000;
#endif

void loop () {
#if WITH_CYCLE_REPORT
  unsigned long loopStart = micros ();
#endif

  Kaleidoscope.loop();

  if (algernon::TapDance::cancelOneShot) {
    if (! Layer.isOn (_APPSEL))
      OneShot.cancel ();
    algernon::TapDance::cancelOneShot = false;
  }

#if WITH_CYCLE_REPORT
  unsigned long loopTime = micros () - loopStart;

  if (avgLoopTime)
    avgLoopTime = (avgLoopTime + loopTime) / 2;
  else
    avgLoopTime = loopTime;

  if (millis () >= nextReport) {
    if (algernon::Settings::settings.cycleTimer) {
      Serial.print (F("avgLoopTime: "));
      Serial.println (avgLoopTime);
    }
    nextReport = millis() + 1000;
  }
#endif
}
