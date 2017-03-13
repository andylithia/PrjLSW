contents:
* top level sch: LSW_0_HCMS29_STM32L0.SchDoc
  - LSWM_00_HCMS29_Lithium.SchDoc
    Lithum battery charge interface, display module, regulator
  - LSWM_10_STM32L0_ADXL345.SchDoc
    STM32L0 controller, ADXL345 accelerometer
* PCB implementation #0 : LSW0_HCMS29_STM32L0_Prototype.PcbDoc

Todo:
* Move ADXL345 to the bottom board
* Add CH340 to the top board
* Add HDSP2000 interface (might be difficult due to opposite power pins)
* Add battery voltage monitor
* Combine all used libraries