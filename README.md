# SmartMailBox
Arduino/ESP8266 project for a "smart" mailbox that posts a message to an MQTT feed 

I made this project because I need a system that sends me an alert whenever the postman leaves new mail in my mailbox 
(yes, I mean the snail mail, this has nothing to do with the e-mail). 
To do this, I added a small magnet to the top lid of the mail box and an Hall-effect IC inside it that resets an ESP01 board 
powered on battery. Each time the lid is opened and closed, the ESP01 is reset, so when it boots up again, connects to my WiFi, 
posts a message to an MQTT broker and enters the Deep Sleep Mode for longer battery life.

See the attached schematics and Arduino sketch.

