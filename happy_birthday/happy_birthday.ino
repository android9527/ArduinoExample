/*
Happy Birthday Tone for Daner.
Made by TAHO on 6th,Nov. 2013
www.tahoroom.com
[email]tahoroom@163.com[/email]
version 1.0
 */
 #include "pitches.h"
 
void setup() {
  // iterate over the notes of the melody:
  pinMode(9,OUTPUT);//for light
  pinMode(10,OUTPUT);//for light
  pinMode(11,OUTPUT);//for light
}
 
void loop() {
  // no need to repeat the melody.
  ColorLight();//Turn on the lights
  play();//Play the music.
  delay(300);//Pause for a while.
}
 
// notes in the melody:
int melody[] = {
NOTE_G4,//5  
NOTE_G4,//5
NOTE_A4,//6
NOTE_G4,//5
NOTE_C5,//1.
NOTE_B4,//7
0,
NOTE_G4,//5
NOTE_G4,//5
NOTE_A4,//6
NOTE_G4,//5
NOTE_D5,//2.
NOTE_C5,//1.
0,
NOTE_G4,//5
NOTE_G4,//5
NOTE_G5,//5.
NOTE_E5,//3.
NOTE_C5,//1.
NOTE_B4,//7
NOTE_A4,//6
0,
NOTE_F5,//4.
NOTE_F5,//4.
NOTE_E5,//3.
NOTE_C5,//1.
NOTE_D5,//2.
NOTE_C5,//1.
0,
};
 
 
int noteDurations[] = {
  8,8,4,4,4,4,
  4,
  8,8,4,4,4,4,
  4,
  8,8,4,4,4,4,2,
  8,
  8,8,4,4,4,2,
  4,
};
 
 
void play()
{
    for (int thisNote = 0; thisNote < 29; thisNote++) {
 
    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(5, melody[thisNote],noteDuration);
 
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.40;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}
 
void ColorLight()
{
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
}

