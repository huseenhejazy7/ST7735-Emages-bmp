
    //C++
   // ALM ALTOKNOLOGIA & Engineer HOUSSEIN HIJAZI.

#include <SPI.h>
#include <SD.h>
#include <TFT.h> 

#define PIN_SD_CS  4
#define PIN_TFT_CS 10
#define PIN_DC     8
#define PIN_RST    9

#define DELAY_IMAGE_SWAP 2000 // each image is shown for 60 seconds

TFT TFTscreen = TFT(PIN_TFT_CS, PIN_DC, PIN_RST);

void setup() {
  // initialize default serial connection to send debug information
  Serial.begin(9600);
  while (!Serial) {
    // wait until default serial connection is fully set up
  }
  
  //The following two lines replace "TFTscreen.begin();" => avoids that red and blue (?) are swapped/interchanged
  TFTscreen.initR(INITR_BLACKTAB);
  TFTscreen.setRotation(1);
  
  
  TFTscreen.background(255, 255, 255); // prints black screen to TFT display
  
  init_SD(); // function call that initializes SD card
}

void init_SD() {
  // try to init SD card
  Serial.print(F("SD card init..."));
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(F("ERROR")); // failed
    return;
  }
  Serial.println(F("SUCCESS .ABO NBEEL... OK")); // ok
}

void loop() {
  File dir = SD.open("/"); // open root path on SD card
  File entry;
  char name[16];
  bool worked_once = false;

  while (entry = dir.openNextFile()) { // iteratively opens all files on SD card. 
    Serial.print(F("Opened File: "));
    Serial.println(entry.name());
    strcpy(name, entry.name()); // file name is copied to variable "name"
    entry.close(); // After copying the name, we do not need the entry anymore and, therefore, close it.
    int filename_len = strlen(name);
    if ((filename_len >= 4 && strcmp(name + filename_len - 4, ".BMP") == 0)) { // check if the current filename ends with ".BMP". If so, we might have an image.
      PImage image = TFTscreen.loadImage(name); // loads the file from the SD card
      if (image.isValid()) { // If the loaded image is valid, we can display it on the TFT.
        Serial.println(F("Image is valid... drawing image."));  
        TFTscreen.image(image, 0, 0); // this function call displays the image on the TFT. 
        worked_once = true; // we set this variable to true, in order to indicate that at least one image could be displayed
        delay(DELAY_IMAGE_SWAP);
      } else {
        Serial.println(F("Image is not valid... image is not drawn."));  
      }  
      image.close(); // image is closed as we do not need it anymore.
    } else {
      Serial.println(F("Filename does not end with BMP!"));  
    }
  }
  dir.close(); // directory is closed


  if (worked_once == false) { // if not a single image could be shown, we reconnect to the SD card reader.
    Serial.println(F("Warning: Printing an image did not work once! Trying to reinitalize SD card reader."));        
    SD.end();
    init_SD();
  }

}
