# icehat

icehat is a small (Zero-sized) RPi hat which comprises a Lattice iCE40 Ultra
FPGA type ice5LP4K-SG48 along with voltage regulators, status LEDs and three
Digilent-style PMOD connectors.

This repository includes the full KiCAD board design source as well as C source
for an RPi user application which can configure the FPGA via GPIO and SPI ports
as well as communicate with a simple SPI slave hosted on the FPGA. Verilog
source for the FPGA test design is also included.

Credits: 

Portions of the KiCAD design are based on the RPi_Hat_Template from
xesscorp: https://github.com/xesscorp/RPi_Hat_Template
