/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
#define CLK 13       // SPI Clock, shared with SD card
#define MISO 34      // Input data, from VS1053/SD card
#define MOSI 26      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
/*
#define BREAKOUT_RESET  21      // VS1053 reset pin (output)
#define BREAKOUT_CS      32     // VS1053 chip select pin (output)
#define BREAKOUT_xDCS    33      // VS1053 Data/command select pin (output)
#define SDCS 22     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 35       // VS1053 Data request, ideally an Interrupt pin
*/
#define BREAKOUT_RESET  -1      // VS1053 reset pin (output)
#define BREAKOUT_CS      4     // VS1053 chip select pin (output)
#define BREAKOUT_xDCS    12      // VS1053 Data/command select pin (output)
#define SDCS 33     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 36      // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(MOSI, MISO, CLK, BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_xDCS, DREQ, SDCS);
//  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_xDCS, DREQ, SDCS);

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(SDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
  // Play one file, don't return until complete
/*  Serial.println(F("Playing track 001"));
  musicPlayer.playFullFile("/0001.mp3");
  // Play another file in the background, REQUIRES interrupts!
  Serial.println(F("Playing track 002"));
  musicPlayer.startPlayingFile("/0002.MP3");
*/}

void loop() {
  static boolean bPlaying = false;
  // feed the buffer if playing otherwise poll every .1s
  if (!musicPlayer.stopped())
    bPlaying = true;
  else {
   if (bPlaying == true) {
     Serial.println("Done playing music");
     bPlaying = false;
   }
  }


  if (Serial.available()) {
    char c = Serial.read();
    
    if (c == 't') {
      musicPlayer.startPlayingFile("/0001.MP3");

      //play a tone
      //musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
    }

    // if we get an 's' on the serial console, stop!
    if (c == 's') {
      musicPlayer.stopPlaying();
    }
    
    // if we get an 'p' on the serial console, pause/unpause!
    if (c == 'p') {
      if (! musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.pausePlaying(true);
      } else { 
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
      }
    }
    static int iVol = 20;
    if (c == '+') {
      // Set volume for left, right channels. lower numbers == louder volume!
      if (iVol > -1) {
        iVol--;
        musicPlayer.setVolume(iVol,iVol);
      }
    }
    if (c == '-') {
      // Set volume for left, right channels. lower numbers == louder volume!
      if (iVol <= 255) {
        iVol++;
        musicPlayer.setVolume(iVol,iVol);
      }
    }
  }
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
