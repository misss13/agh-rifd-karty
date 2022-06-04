#include "UART.h"

char UART::buffer[UARTBufferLength]; 
byte UART::bufferLen = 0;
bool UART::cleared = false;
int UART::WLANCZNIK = 0;

void UART::init() {
  Serial.begin(9600);
}

void UART::tick() {
  if (Serial.available() <= 0)
    return;
  
  int sr = Serial.read();

  if (sr == 27) { //ESC key
    clear();
    memset(buffer, '\0', UARTBufferLength);
    bufferLen = 0;
    cleared = false;
    return;
  }
  
  if (sr == '\n' || sr == '\r' || sr == '\0') {
    if (bufferLen == 0)
      return;

    clear();
    
    Serial.printf("\033[35m>>\033[39m %s\r\n", buffer);
    process_command();
    memset(buffer, '\0', UARTBufferLength);
    bufferLen = 0;
    cleared = false;
    return;
  }

  if (sr < 32 || sr > 126)
    return;

  if (sr == '\b') {
    if (bufferLen == 0)
      return;
      
    bufferLen--;
    buffer[bufferLen] = '\0';
    
    if (cleared)
      return;
      
    Serial.print("\b \b");
    return;
  }

  if (bufferLen >= UARTBufferLength)
    return;

  buffer[bufferLen] = (char)sr;
  bufferLen++;

  if (cleared)
    return;
  
  Serial.print((char)sr);
}

void UART::clear() {
  cleared = true;
  for (char i = 0; i < bufferLen; i++)
    Serial.print("\b \b");
}

void UART::restore() {
  Serial.printf("%s", buffer);
  cleared = false;
}

void UART::println(String s) {
  if (cleared) {
    Serial.println(s);
    return;
  }
  
  clear();
  Serial.println(s);
  restore();
}

void UART::print_error(String s) {
  println("\033[31m" + s + "\033[39m");
}

void UART::print_warning(String s) {
  println("\033[33m" + s + "\033[39m");
}

void UART::print_success(String s) {
  println("\033[32m" + s + "\033[39m");
}

void UART::process_command() {
  String command = "";
  byte args;
  
  for (args = 0; args < UARTBufferLength && buffer[args] != '\0' && buffer[args] != ' '; args++)
    command += buffer[args];

  if (args + 1 < UARTBufferLength)
    args++;

  if (command == "testcommand") {
	print_success("It works!");
    return;
  }

  if (command == "testcommand2") {
    print_error("It doesn't work!");
    return;
  }
  if (command == "add_card") {
    if ( WLANCZNIK == 0 ){
      println("Add card - on");
      WLANCZNIK = 1;
    } else {
      println("Add card - off");
      WLANCZNIK = 0;
    }
    return;
  }

}