#ifndef EFFECTS_ENUM_H
#define EFFECTS_ENUM_H

enum EffectsEnum {
  Mii,
  Estatico,
  Blink,
  Cyclon,
  ColorWipe,
  ColorWipeReverse,
  SnowSparkle,
  Rainbow,
  // New Effects
  EFFECTS_COUNT
};

inline const char *toString(EffectsEnum effect) {
  switch (effect) {
  case Mii:
    return "Mii";
  case Estatico:
    return "Estatico";
  case Blink:
    return "Blink";
  case Cyclon:
    return "Cyclon";
  case ColorWipe:
    return "ColorWipe";
  case ColorWipeReverse:
    return "ColorWipeReverse";
  case SnowSparkle:
    return "SnowSparkle";
  case Rainbow:
    return "Rainbow";
  default:
    return "DESCONHECIDO";
  }
}

inline EffectsEnum fromString(const String &effectName) {
  if (effectName == "Estatico")
    return Estatico;
  if (effectName == "Blink")
    return Blink;
  if (effectName == "Cyclon")
    return Cyclon;
  if (effectName == "ColorWipe")
    return ColorWipe;
  if (effectName == "ColorWipeReverse")
    return ColorWipeReverse;
  if (effectName == "SnowSparkle")
    return SnowSparkle;
  if (effectName == "Rainbow")
    return Rainbow;
  return static_cast<EffectsEnum>(
      -1); // Retorna um valor inválido se a string não for encontrada
}

#endif
