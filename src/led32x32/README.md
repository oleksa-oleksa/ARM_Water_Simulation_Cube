# LED Matrix Panel

## Detailed Tutorial
- See [wiki][7]

## Reference Project
- LedGames - a BeagleBone Black 64x64 LED Game (see [detail][1])
- 32x16 and 32x32 RGB LED Matrix (see [detail][2])
- RGB Matrix FeatherWing (see [detail][3])
- Max clock rate test for panel (see [detail][4])
- Control flow (see [here][8])
## Reference Library
- [Adafruit-GFX-Library][5]
- [RGB-matrix-Panel][6]

[7]: https://bikerglen.com/projects/lighting/led-panel-1up/#Required_Software
[1]: https://learn.adafruit.com/ledgames-beaglebone-black-64x64-led-game/overview
[2]: https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/how-the-matrix-works
[3]: https://learn.adafruit.com/rgb-matrix-featherwing/overview
[4]: https://forums.adafruit.com/viewtopic.php?f=47&t=26130&start=0
[5]: https://github.com/adafruit/Adafruit-GFX-Library
[6]: https://github.com/adafruit/RGB-matrix-Panel

[8]: https://qiita.com/onokatio/items/1b99ae9475b6a9fc2f15

## (WIP) Binary Code Manipulation for Color Intensity
> TODO complete this section

- 3 bits * 2 parts (top,bottom) * 4 panels = 24 bits = 3 bytes
- 32 pixels in a row
- 16 rows to control

that result in that we need totally 3-byte * 32 * 16 = 1536 Byte to store color information for only one image.

## Dev Note
Panel information: integer 32-bit per pixel  
Chained panels are considered as if the width becomes larger.
