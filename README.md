# Raspberry Pi + L6470 
## 3A RMS Stepper Motor Control

This repo has a ton of options, but we're just here for one particular chip: L6470. Rather than mucking through the whole thing to figure out what I can delete, I'll leave all the extras here for now and trim it down later. You can find more infomation on my research process and intended application on [my website](http://taylorhokanson.com/2018/12/26/driving-a-higher-amperage-stepper-motor-w-raspberry-pi/).

### Hardware

1. Raspberry Pi (tested version 3)
2. EVAL6470H

!["wiring"](https://github.com/TaylorHokanson/rpidmx512/blob/master/media/wiring.jpg "wiring")

### Software

1. Install and update Raspbian
2. `sudo raspi-config`
   * Interfacing options
      * enable SPI
      * enable SSH
3. Download and install [this library](http://www.airspayce.com/mikem/bcm2835/)
4. Download and extract this repo to the Desktop
5. `cd /home/pi/Desktop/rpidmx512/lib-l6470/examples/sparkfun`
6. `make`
7. `sudo ./SetParam`
   * these settings work for my 2.1A stepper @ 12V, but YMMV
8. `sudo ./simple_move`
