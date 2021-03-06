#include <SoftwareSerial.h>

#include <TinyGPS.h>

/*#define RXPIN 4 //From GPS to Arduino
#define TXPIN 5 //From Arduino to GPS
<<<<<<< HEAD:SandStorm2.ino
=======
SoftwareSerial goopsSerial(4, 5);*/
>>>>>>> Made the program that connects to goops:SandStorm2/SandStorm2.ino

TinyGPS gps;
SoftwareSerial nss(2, 3);
SoftwareSerial goopsSerial(4, 5);

void gpsdump(TinyGPS &gps);
bool feedgps();
void printFloat(double f, int digits = 2);
int cutoffInstruction;

void setup()
{
  
  /*These numbers are crucial! The Serial ports of the Xstream modems 
  is at a baud rate of 115200 bps. The serial port of the GPS receiver
  is at a baud rate of 57600 bps.
  */
  
  Serial.begin(115200);
  nss.begin(57600);
  /*goopsSerial.begin(57600);*/
  
<<<<<<< HEAD:SandStorm2.ino
  Serial.println("SandStorm 1 Location Tracker ");
=======
  Serial.print("SandStorm 1 Location Tracker ");
>>>>>>> Made the program that connects to goops:SandStorm2/SandStorm2.ino
  Serial.println("by Guillermo Vargas");
  Serial.println();
  Serial.print("Sizeof(gpsobject) = "); Serial.println(sizeof(TinyGPS));
  Serial.println("Please wait, acquiring GPS fix and parsing data...");
  Serial.println();

  //Setup the output pins for the camera and cutoff systems.
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop()
{
  
  //Instruction to listen for action requests.
  
  if(Serial.available()>0) {
    cutoffInstruction = Serial.read();
    if(cutoffInstruction == 50) {
      Serial.print("Instruction Received, Cuttoff System On, Cutoff Underway");
      Serial.println();
      digitalWrite(8, HIGH);
      delay(10000);
      digitalWrite(8, LOW);
      Serial.flush();
    }
  }
  // ------------------------------------------
  
  //Commands to send through Serial the GPS coordinates of the probe to Goops.
<<<<<<< HEAD:SandStorm2.ino
  
  while (goopsSerial.available()) {
    int c = goopsSerial.read();
    if(gps.encode(c)){
      println(c);
    }
  }

=======

  /*if(goopsSerial.available()){
    Serial.write(goopsSerial.read());
  }*/
>>>>>>> Made the program that connects to goops:SandStorm2/SandStorm2.ino
  
  //--------------------------------------------
  
  //Periodic request to take picture every 5000 ms.
  
  Serial.print("Taking Picture");
  Serial.println();
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(13, LOW);
  delay(5000);
  
  // -------------------------------------------
  bool newdata = false;
  unsigned long start = millis();

  // Every 5 seconds we print an update
  while (millis() - start < 1000)
  {
    if (feedgps())
      newdata = true;
  }
  
  if (newdata)
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump(gps);
    Serial.println("-------------");
    Serial.println();
  }
}


void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint; 
  } 
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);
  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
  
  feedgps(); // If we don't feed the gps during this long routine, we may drop characters and get checksum errors

  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  feedgps();

  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): "); Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  feedgps();

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour)); Serial.print(":"); Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second)); Serial.print("."); Serial.print(static_cast<int>(hundredths));
  Serial.print("  Fix age: ");  Serial.print(age); Serial.println("ms.");
  
  feedgps();

  Serial.print("Alt(cm): "); Serial.print(gps.altitude()); Serial.print(" Course(10^-2 deg): "); Serial.print(gps.course()); Serial.print(" Speed(10^-2 knots): "); Serial.println(gps.speed());
  Serial.print("Alt(float): "); printFloat(gps.f_altitude()); Serial.print(" Course(float): "); printFloat(gps.f_course()); Serial.println();
  Serial.print("Speed(knots): "); printFloat(gps.f_speed_knots()); Serial.print(" (mph): ");  printFloat(gps.f_speed_mph());
  Serial.print(" (mps): "); printFloat(gps.f_speed_mps()); Serial.print(" (kmph): "); printFloat(gps.f_speed_kmph()); Serial.println();

  feedgps();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: "); Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);
}
  
bool feedgps()
{
  while (nss.available())
  {
    if (gps.encode(nss.read()))
      return true;
      }
  return false;
}
