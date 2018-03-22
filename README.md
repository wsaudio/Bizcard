# Bizcard
Drum machine business card: STM32F030 Based drum machine with 4 sounds and 16 pads

FOLDERS
Hardware:
	- Schematics: PDF of schematics
	- Gerbers: Gerber and drill files required for manufacturer
	- Project Files: KiCad V4.0.0 (or later) required to open
	- BOM: Bill of materials required for manufacture
Software: Created with Keil uVision 5.0

Pins configuration:
PA7: Audio output (TIM1 PWM1)
PA0, PA1, PA2, PA3, PA4: LED/Button MUX Out
PA5, PA6, PA9, PA10: LED MUX In
PA13, PA14, PF0, PF1: Button MUX In