/* -*- mode: c++ -*-
 * Model01-Sketch -- algernon's Model01 Sketch
 * Copyright (C) 2016  Gergely Nagy
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

#include "EventHandler.h"

#define KC_DQT LSFT(KC_QUOT)

static const uint8_t _hun_key_map[] PROGMEM = {
  KC_A, // HU_AA
  KC_O, // HU_OO
  KC_O, // HU_OE
  KC_O, // HU_OEE
  KC_E, // HU_EE
  KC_U, // HU_UU
  KC_U, // HU_UE
  KC_U, // HU_UEE
  KC_I, // HU_II
};

static const uint16_t _hun_accent_map[] PROGMEM = {
  KC_QUOT, // HU_AA
  KC_QUOT, // HU_OO
  KC_DQT,  // HU_OE
  KC_EQL,  // HU_OEE
  KC_QUOT, // HU_EE
  KC_QUOT, // HU_UU
  KC_DQT,  // HU_UE
  KC_EQL,  // HU_UEE
  KC_QUOT, // HU_II
};

static const uint8_t LayerBaseColorMap[][64] PROGMEM = {
  [DVORAK] = {
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
  },
  [ADORE] = {
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
  },
  [NAV] = {
    0, 1, 1, 1, 1, 1, 0,  9, 0,  0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0,  9,10,  0, 0,10,10,10, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0,10,  0, 0,10,10,10, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  9, 0,  0, 0, 0, 0, 0, 0, 0,
  },
  [HUN] = {
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0, 0, 0, 0, 0, 0,
  },
};

namespace algernon {
  static const cRGB colorMap[] PROGMEM = {
    [Color::OFF]      = {0, 0, 0},
    [Color::RED]      = {0xff, 0, 0},
    [Color::GREEN]    = {0, 0x80, 0},
    [Color::BROWN]    = {0xa5, 0x2a, 0x2a},
    [Color::BLUE]     = {0, 0, 0xff},
    [Color::PURPLE]   = {0x80, 0, 0x80},
    [Color::GREY]     = {0x80, 0x80, 0x80},
    [Color::LIGHTRED] = {0xff, 0x66, 0x66},
    [Color::LIME]     = {0x00, 0xff, 0},
    [Color::YELLOW]   = {0xff, 0xff, 0},
    [Color::CYAN]     = {0, 0xff, 0xff},
    [Color::PINK]     = {0xff, 0xc0, 0xcb},
    [Color::WHITE]    = {0xff, 0xff, 0xff},
  };

  void
  EventHandler::setColor (uint8_t index, int colorIndex) {
    cRGB color = getColor (colorIndex);

    if (LEDColors[index] != colorIndex) {
      LEDColors[index] = colorIndex;
      M01::EventHandler::Full::setColor (index, color);
    }
  }

  cRGB
  EventHandler::getColor (uint8_t colorIndex) {
    cRGB color;

    color.r = pgm_read_byte (colorMap + colorIndex * 3);
    color.g = pgm_read_byte (colorMap + colorIndex * 3 + 1);
    color.b = pgm_read_byte (colorMap + colorIndex * 3 + 2);

    return color;
  }

  void
  EventHandler::blendColor (uint8_t index, int colorIndex) {
    cRGB color = getColor (LEDColors[index]);

    if (colorIndex == Color::OFF) {
      // set the original color...
    } else {
      cRGB add = getColor (colorIndex);

      // compute the combined base + add color
      color.r = min(color.r + add.r, 255);
      color.g = min(color.g + add.g, 255);
      color.b = min(color.b + add.b, 255);
    }

    M01::EventHandler::Full::setColor (index, color);
  }

  void
  EventHandler::press (uint8_t index) {
    uint16_t keycode = keymap->lookup (index);

    blendColor (index, Color::GREY);

    if (keycode == KC_ESC) {
      if (Akela::TapDance::Component::OneShotMod::isOneShotActive ()) {
        Akela::TapDance::Component::OneShotMod::cancelOneShot (HID, keymap);
        return;
      }
      if (Akela::TapDance::Component::OneShotLayer::isOneShotActive ()) {
        Akela::TapDance::Component::OneShotLayer::cancelOneShot (HID, keymap);
        return;
      }
    }

    if (Akela::TapDance::Component::OneShotMod::register_code (HID, keymap, keycode))
      return;
    if (Akela::TapDance::Component::OneShotLayer::register_code (HID, keymap, keycode))
      return;

    M01::EventHandler::Full::press (index);
  };

  void
  EventHandler::release (uint8_t index) {
    uint16_t keycode = keymap->lookup (index);

    // change color only if it was grey-ish
    blendColor (index, Color::OFF);

    if (Akela::TapDance::Component::OneShotMod::unregister_code (HID, keymap, keycode))
      return;
    if (Akela::TapDance::Component::OneShotLayer::unregister_code (HID, keymap, keycode))
      return;

    M01::EventHandler::Full::release (index);
  }

  void
  EventHandler::colorModifiers () {
    static const uint8_t modIndexMap[] PROGMEM = {
      57, // KC_LCTL
      39, // KC_LSFT
      38, // KC_LALT
      55, // KC_LGUI
      57, // KC_RCTL
      39, // KC_RSFT
      38, // KC_RALT
      55, // KC_RGUI
    };

    for (uint8_t kc = KC_LCTL; kc <= KC_RGUI; kc++) {
      int color = Color::OFF;
      uint8_t modIndex = pgm_read_byte (modIndexMap + kc - KC_LCTL);

      if (HID->isModifierActive (kc)) {
        switch (kc) {
        case KC_LCTL:
        case KC_RCTL:
          color = Color::BLUE;
          break;

        case KC_LSFT:
        case KC_RSFT:
          color = Color::RED;
          break;

        case KC_LALT:
        case KC_RALT:
          color = Color::GREEN;
          break;

        case KC_LGUI:
        case KC_RGUI:
          color = Color::BROWN;
          break;
        }
      }

      setColor (modIndex, color);
    }
  }

  void
  EventHandler::colorLayers () {
    M01::KeyMap *km = (M01::KeyMap *)keymap;
    uint8_t layer = km->layer ();

    for (uint8_t i = 0; i < 64; i++) {
      uint8_t colorIndex = pgm_read_byte (LayerBaseColorMap + layer * 64 + i);

      setColor (i, colorIndex);
    }
  }

  void
  EventHandler::loop () {
    Akela::TapDance::Component::OneShotMod::loop (HID, keymap);
    M01::EventHandler::Full::loop ();

    colorLayers ();
    colorModifiers ();
  }

  void
  EventHandler::macroAction (Akela::AbstractHID *hid,
                             Akela::KeyMap *,
                             uint8_t macroIndex,
                             bool pressed) {
    switch (FM(macroIndex)) {
    case AM_1 ... AM_0:
      return handleNums (hid, macroIndex, pressed);
    case HU_AA ... HU_II:
      return handleHun (hid, macroIndex, pressed);
    case AM_MNP:
      return handleMediaPrevNext (hid, pressed);
      break;
    }
  }

  void
  EventHandler::handleMediaPrevNext (Akela::AbstractHID *hid,
                                     bool pressed) {
    if (!pressed)
      return;

    if (hid->isModifierActive (KC_LSFT)) {
      bool needShiftRestore = true;

      if (Akela::TapDance::Component::OneShotMod::isOneShotModifierActive (KC_LSFT)) {
        needShiftRestore = false;
        Akela::TapDance::Component::OneShotMod::cancelOneShot (HID, keymap);
      }

      hid->release (KC_LSFT);

      M01::EventHandler::Full::register_code (CC_PRV);
      hid->sendReport ();
      M01::EventHandler::Full::unregister_code (CC_PRV);

      if (needShiftRestore)
        hid->press (KC_LSFT);

      return;
    }

    M01::EventHandler::Full::register_code (CC_NXT);
    hid->sendReport ();
    M01::EventHandler::Full::unregister_code (CC_NXT);
  }

  void
  EventHandler::handleHun (Akela::AbstractHID *hid,
                           uint8_t macroIndex,
                           bool pressed) {
    bool needShift = false;
    bool holdShift = false;
    uint16_t accent;
    uint8_t key;

    if (!pressed)
      return;

    if (hid->isModifierActive (KC_LSFT)) {
      needShift = holdShift = true;
      hid->release (KC_LSFT);
    }
    if (Akela::TapDance::Component::OneShotMod::isOneShotModifierActive (KC_LSFT)) {
      needShift = true;
      holdShift = false;

      Akela::TapDance::Component::OneShotMod::cancelOneShot (HID, keymap);
    }

    // select accent / key
    key = pgm_read_byte (_hun_key_map + FM (macroIndex) - HU_AA);
    accent = pgm_read_word (_hun_accent_map + FM (macroIndex) - HU_AA);

    // do the input
    hid->press (KC_RALT);
    hid->sendReport ();
    hid->release (KC_RALT);

    if (accent == (KC_DQT)) {
      hid->press (KC_RSFT);
    }
    hid->press (accent);
    hid->sendReport ();
    hid->release (accent);
    if (needShift && accent != (KC_DQT)) {
      hid->press (KC_RSFT);
    } else if (accent == (KC_DQT) && !needShift) {
      hid->release (KC_RSFT);
    }
    hid->sendReport ();
    hid->press (key);
    hid->sendReport ();
    hid->release (key);
    if (needShift || accent == (KC_DQT))
      hid->release (KC_RSFT);
    if (holdShift)
      hid->press (KC_LSFT);

    hid->sendReport ();
  }

  void
  EventHandler::handleNums (Akela::AbstractHID *hid,
                            uint8_t macroIndex,
                            bool pressed) {
    uint8_t kc = 0;
    static bool shifted[10];

    if (hid->isModifierActive (KC_LSFT) && pressed)
      shifted[macroIndex] = true;

    if (!shifted[macroIndex]) {
      kc = macroIndex + KC_1;
    } else {
      switch (FM(macroIndex)) {
      case AM_8:
      case AM_9:
        shifted[macroIndex] = false;
        return;

      case AM_7:
        kc = KC_2;
        break;
      case AM_5:
        kc = KC_8;
        break;
      case AM_3:
        kc = KC_6;
        break;
      case AM_1:
        kc = KC_4;
        break;

      case AM_0:
        kc = KC_5;
        break;
      case AM_2:
        kc = KC_1;
        break;
      case AM_4:
        kc = KC_3;
        break;
      case AM_6:
        kc = KC_7;
        break;
      }
    }

    if (pressed) {
      hid->press (kc);
    } else {
      hid->release (kc);
      shifted[macroIndex] = false;
    }

  }

  void
  EventHandler::tapDanceFinish (Akela::AbstractHID *hid,
                                Akela::KeyMap *,
                                uint8_t tapIndex,
                                uint8_t counter) {
    switch (tapIndex) {
    case TC_TMX:
      if (counter == 1) {
        hid->press (KC_RALT);
        hid->press (KC_SPC);
      } else {
        hid->press (KC_RCTL);
        hid->press (KC_A);
      }
      break;
    case TC_TMXP:
      hid->press (KC_RALT);
      hid->press (KC_SPC);
      hid->sendReport ();
      hid->release (KC_RALT);
      hid->release (KC_SPC);
      hid->sendReport ();
      if (counter == 1) {
        hid->press (KC_P);
      } else {
        hid->press (KC_Z);
      }
      break;

    case TC_LPB:
      if (counter == 1) {
        hid->press (KC_LBRC);
      } else {
        hid->press (KC_RSFT);
        hid->press (KC_9);
      }
      break;
    case TC_RPB:
      if (counter == 1) {
        hid->press (KC_RBRC);
      } else {
        hid->press (KC_RSFT);
        hid->press (KC_0);
      }
      break;

    case TC_CLN:
      if (counter == 1) {
        hid->press (KC_RSFT);
      }
      hid->press (KC_SCLN);
      break;

    case TC_GUI:
      hid->press (KC_LGUI);
      if (counter == 2) {
        Serial.print (F("CMD:appsel_start"));
      }
      break;
    }
  }

  void
  EventHandler::tapDanceRelease (Akela::AbstractHID *hid,
                                 Akela::KeyMap *,
                                 uint8_t tapIndex,
                                 uint8_t counter) {
    switch (tapIndex) {
    case TC_TMX:
      if (counter == 1) {
        hid->release (KC_RALT);
        hid->release (KC_SPC);
      } else {
        hid->release (KC_RCTL);
        hid->release (KC_A);
      }
      break;
    case TC_TMXP:
      if (counter == 1) {
        hid->release (KC_P);
      } else {
        hid->release (KC_Z);
      }
      break;

    case TC_LPB:
      if (counter == 1) {
        hid->release (KC_LBRC);
      } else {
        hid->release (KC_RSFT);
        hid->release (KC_9);
      }
      break;
    case TC_RPB:
      if (counter == 1) {
        hid->release (KC_RBRC);
      } else {
        hid->release (KC_RSFT);
        hid->release (KC_0);
      }
      break;

    case TC_CLN:
      if (counter == 1) {
        hid->release (KC_RSFT);
      }
      hid->release (KC_SCLN);
      break;

    case TC_GUI:
      hid->release (KC_LGUI);
      break;
    }
  }

};
