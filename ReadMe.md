# Smart Reverse #
Smart Reverse POC which can be easily used in any vehicle.


## Features ##
* Generate a newly random password, which is accessible using the Blynk app.
* Measure live distance from other vehicles on real-time and notify the customer
  * LCD screen indicative message
  * Buzzer beeps corresponding to the distance
  * LED matrix with corresponding smiley
* Using Blynk cloud service to show useful information
    * Current password
    * Current distance to crash
    * Email for notifications
    * GPS coordinates
    * Distance history graph for further analysis
* Upon crash, email with GPS location will be sent to the customer


## Examples ##

### Blynk mail dashboard ###
![BLYNK_mail_dashboard](doc_img/BLYNK_mail_dashboard.jpg "Blynk Mail Dashboard")

### Getting close ###
![getting_close](doc_img/Poker_Face.jpg "Getting close")

### Smart Reverse Back ###
![smart_reverse_back](doc_img/SmartReverse_Back.jpg "Smart Reverse Back")

### Wrong Password ###
![wrong_password](doc_img/Wrong_Password.jpg "Wrong Password")


## Hardware Requirements ##
* Arduino UNO
* Ultrasonic sensor
* LCD 16x2
* Keypad 4x4
* Buzzer
* Led matrix 8x8
* Jumper wires 
* USB Arduino cable
