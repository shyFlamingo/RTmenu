/*
 * Real Time Menu
 * 
 * Chad Sprong 
 * 
 * version 1.0
 * 
 * Example of near real time menu using an arduino mega.
 * 
 */

 #define RDA 0x80
 #define TBE 0x20  
 volatile unsigned char *portB = (unsigned char *) 0x25;
 volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
 volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
 volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
 volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
 volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 unsigned char inputChar = 0;
 int delayTime = 1000;
 unsigned long cursCount = 0;
 unsigned long cursChange = 100000;
 int changes = 0;




void setup() {
 // initialize the serial port on USART0:
 U0init(9600);
 delay(1000);
}



void loop() {
  inputChar =  0;
  resetScreen();
  makeMenu();
  sysWrk();
}
/*
 * surprise function that draws an image onto the terminal 
 */
void surprise(){
  resetScreen();
  Serial.println("This is the moster of the program...");
  Serial.println("He lives inside your computer...");
  Serial.println("Pressing escape returns to the main menu...");
  Serial.println();
  Serial.println("|(|)|(|)|(|)|(|)|(|)|(|)|(|)|");
  Serial.println("|    __                __   |");
  Serial.println("<   ..@                @..   >");
  Serial.println("\\                            /");
  Serial.println("\\          XXXXXXXX         /");
  Serial.println(" \\                         /  ");
  Serial.println("         ^^^^^^^^^^^^^^^       ");
  while( inputChar != 27 ){
    while (U0kbhit()==0){     makeCurs(); };  // wait for RDA = true
    inputChar = U0getChar();    // read character
  }
  resetScreen();
  return;
}

/*
 * changes the speed of the cursor 
 */
void cursSpeed(){
  resetScreen();
  Serial.println("This is the cursor speed portion of the program...");
  Serial.println("This program changes the speed of the cursor...");
  Serial.println("Pressing escape returns to the main menu...");
  while( inputChar != 27 ){
    while (U0kbhit()==0){     makeCurs(); };  // wait for RDA = true
    inputChar = U0getChar();    // read character
    switch( inputChar ){
      case '-':
        cursChange += 3000;
        break;
      case '+':
        cursChange -= 3000;
        break;
      default:
        clearLine();
        putstr("      Please input number either - or + ");
        break;
    }
  }
  resetScreen();
}

void echoHex(){
  resetScreen();
  Serial.println("This is the hex key echo program...");
  Serial.println("This program prints the hex value of the key entered...");
  Serial.println("Pressing return erases the line...");
  Serial.println("Pressing escape returns to the main menu...");
  int cs2, cs3;
  while (inputChar != 27) {
    while (U0kbhit() == 0) {}; // wait for RDA = true
    if (U0kbhit() == 1)
      inputChar = U0getChar();
    if ( inputChar != 27 ) {
      cs2 = (inputChar / 16) + ')';
      if (cs2 > 9)
        cs2 = cs2 + 7;
      cs3 = (inputChar % 16) + ')';
      cs3 = cs3 + 7;
      if (cs3 > 9)
        cs3 = cs3 + 7;
      clearLine();
      Serial.print("Original char: ");
      Serial.print( (char)inputChar );     // echo original character
      Serial.print(" -------  Hex value: ");
      Serial.print( (char)cs2 );     // echo hex character
      Serial.print( (char)cs3 ); 
      Serial.print(" -------  Dec value: ");
      Serial.print( inputChar );      //echo decimal 
    }
    else{
      resetScreen();
    }
  }
}

void echoChar(){
  resetScreen();
  Serial.println("This is the key echo program...");
  Serial.println("Pressing return erases the line...");
  Serial.println("Pressing escape returns to the main menu...");
  while( inputChar != 27 ){
    while (U0kbhit()==0){};  // wait for RDA = true
    inputChar = U0getChar();    // read character
    if( inputChar == 10 || inputChar == 13 ){
      clearLine();
    }
    else if( inputChar != 27 )U0putChar( inputChar );     // echo character
    else{
      resetScreen();
    }
  }
}

void makeCurs(){
  cursCount++;
  if( cursCount >= cursChange ){
    cursCount = 0;
    changes++;
    if( changes > 3 ) changes = 0;
    switch( changes ){
      case 0:
        clearLine();
        Serial.print("      >--- ");
        break;
      case 1:
        clearLine();
        Serial.print("      ->-- ");
        break;
      case 2:
        clearLine();
        Serial.print("      -->- ");
        break;
      case 3:
        clearLine();
        Serial.print("      ---> ");
        break;
      default:
        break;
    }
  }
}

void sysWrk(){
  while( inputChar != 27 ){
    while (U0kbhit()==0){     makeCurs(); };  // wait for RDA = true
    inputChar = U0getChar();    // read character
    switch( inputChar ){
      case '1':
        echoChar();
        break;
      case '2':
        echoHex();
        break;
      case '3':
       cursSpeed();
       break;
      case '4':
       surprise();
       break;
      case 'q':
        break;
      default:
        clearLine();
        putstr("      Please input number between 1 and 4 inclusive ");
        break;
    }
  }
}

void clearLine(){
  Serial.write(27);       //Print "esc"
  Serial.print("[2K");    //clears line
  Serial.write(27);       //Print "esc"
  Serial.print("[100D");  //moves cursor to the left
}

void makeMenu(){
  Serial.println("Welcome to an example of a real time menu system.");
  Serial.println("You can access the menu options by pressing the corresponding number.");   
  Serial.println("Pressing escape will bring you back to the main menu. ");  
  Serial.println("[1] Key Echo ");  
  Serial.println("[2] Hex Key Echo ");  
  Serial.println("[3] Change Cursor Speed ");  
  Serial.println("[4] Surprise ");   
}

void resetScreen(){
  Serial.write(27);       //Print "esc"
  Serial.print("[2J");    //clears screen
  Serial.write(27);
  Serial.print("[H");     // cursor to home command
}

void putstr(unsigned char *s){
  int i = 0;
  do{
  if (s[i] == 0) break;
  U0putChar(s[i]);
  } 
  while (++i);
 }

/*
 * NAME: u0kbhit
 * DESCRIPTION: checks the RDA bit. returns 1 if there is something
 */
unsigned char U0kbhit(){
  if((*myUCSR0A & 0x80)==0 ) return 0; //if nothing is inuputed
  else return 1;
}

/*
 * NAME: U0getchar
 * DESCRIPTION: returns the information in UDR0
 */
unsigned char U0getChar(){
  return *myUDR0; //return the information inputted and stored
}

// Wait for USART0 TBE to be set then write character to
// transmit buffer

/*
 * NAME: U0putcher
 * DESCRIPTION: puts the charactor onto the screen
 */
void U0putChar(unsigned char U0pdata){
  while((*myUCSR0A & 0x20) == 0 ); //while data is available 
  *myUDR0 = U0pdata;
}

/*
 * NAME: UOinit
 * DESCRIPTION: initializes the USART0. Uses 8 bits of data (int)
 */
void U0init(int U0baud){
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

