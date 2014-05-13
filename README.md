HMC5883L Header Arduino With Auto calibration
========================================

HMC5883L on Arduino with auto calibration capability 

This article is copied from my memo which I wrote for myself to keep track of my casual stuff. This memom is publicly available on http://hobbylogs.me.pn/?p=17#more-17. It is assumed that the reader knows how to connect the pins of HMC5883L breakout board to Arduino, which is very simple.

HMC5883L        Arduino

VCC          -> 5V or 3.3V (HMC5883L is rated for 3.3v but the break out board have a small voltage regulator)
GND          -> GND
SDA          -> SDA
SCL          -> SCL

I wrote a header file for HMC5883L, and the target is to use it as a compass in 2D plane (i.e x and y axis) which is horizontal to the ground. Moreover, the compass points to the magnetic north and NOT the true north as I only require a fixed reference in 2D plane. I hope it will be helpful. If you find any bug please leave a comment. I will look into it as soon as possible.

For detail about the functions please read "Read Me (Compass header file description).pdf" or http://hobbylogs.me.pn/?p=17#more-17
