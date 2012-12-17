/*
 Remote-controlled Servo motor and LED through Web Server
 
 Circuit:
 * Arduino Uno board
 * Ethernet shield attached to pins 10, 11, 12, 13
 * LED attached to pins D2 and D7
 * Servo motors attached to pins D5 and D6
 
 @author Tony Beltramelli <http://www.tonybeltramelli.com>
 
 */

#include <Ethernet.h>
#include <SPI.h>
#include <Servo.h>

const int servoPin1 = 5;
const int servoPin2 = 6;
const int ledPin1 = 2;
const int ledPin2 = 7;
const int duration = 160;

Servo servo1;
Servo servo2;

boolean isReached1 = false;
boolean isReached2 = false;

byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xA3, 0xED };
byte ip[] = { 192, 168, 1, 177 };

const char http200[] = "HTTP/1.1 200 OK";
const char contentTypeTextHtml[] = "Content-Type: text/html; charset=UTF-8";
const char connectionClose[] = "Connection: close";
const char html[] =
  "<html>"
    "<head>"
      "<script type=\"text/javascript\">"
      "var ajax;"
      "try {"
        "ajax = new XMLHttpRequest();"
      "} catch (e) {"
        "try {"
          "ajax = new ActiveXObject('Microsoft.XMLHTTP');"
        "} catch (e) {}"
      "}"
      "function set(c) {"
        "ajax.open('GET', '?action='+c, true);"
        "ajax.send(null);"
      "}"
      "</script>"
      "<style type=\"text/css\">"
        "body {margin: 0, padding: 0}"
        "a {display: block; width: 25%; height: 100%; float:left}"
        ".red {background-color: #f2385a}"
        ".yellow {background-color: #f5a503}"
        ".white {background-color: #e9f1df}"
        ".blue {background-color: #4ad9d9}"
        "input:active {border: solid 10px #a7a7a7}"
      "</style>"
    "</head>"
    "<body>"
        "<a class='red' href='javascript:set(0)'/>"
        "<a class='yellow' href='javascript:set(1)'/>"
        "<a class='white' href='javascript:set(2)'/>"
        "<a class='blue' href='javascript:set(3)'/>"
    "</body>"
  "</html>";

boolean isReading = false;

EthernetServer server(80);

void setup() {

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  EthernetClient client = server.available();

  if(client)
  {
    boolean currentLineIsBlank = true;
    boolean isContentSent = false;

    while (client.connected()) {
      if (client.available()) {
        if(!isContentSent)
        {
          client.println(http200);
          client.println(contentTypeTextHtml);
          client.println(connectionClose);
          client.println();
          client.println(html);
          isContentSent = true;
        }

        char c = client.read();
        
        if(isReading && c == ' ') isReading = false;
        if(c == '?') isReading = true;
        if(isReading){
          if(c == '0')
          {
            if(isReached1) action(servo1, servoPin1, 0);
          }else if(c == '1')
          {
            if(!isReached1) action(servo1, servoPin1, 180);
          }else if(c == '2')
          {
            if(isReached2) action(servo2, servoPin2, 0);
          }else if(c == '3')
          {
            if(!isReached2) action(servo2, servoPin2, 180);
          }
        }
        
        if (c == '\n' && currentLineIsBlank)  break;
        
        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    
    delay(1);
    client.stop();
  }
}

void action(Servo servo, int servoPin, int value)
{
  servo.attach(servoPin);
  servo.write(value);
  delay(duration);
  servo.detach();
  
  switch(servoPin)
  {
    case servoPin1:
      isReached1 = value == 180;
      
      digitalWrite(ledPin1, HIGH);
      delay(duration);
      digitalWrite(ledPin1, LOW);
    break;
    case servoPin2:
      isReached2 = value == 180;
    
      digitalWrite(ledPin2, HIGH);
      delay(duration);
      digitalWrite(ledPin2, LOW);
    break;
  }
}
