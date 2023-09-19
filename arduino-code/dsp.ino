/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 2_First_order_EQ.ino                         |
| This example controls two one-band EQs and   |
| a master volume slider.                      |
|                                              |
| See the SigmaStudio project file if you want |
| to learn more, tweak or do modifications.    | 
|**********************************************/

// Include Wire and SigmaDSP library
#include <Wire.h>
#include "SigmaDSP.h"

// Include generated parameter file
#include "SigmaDSP_parameters.h"

#define EQ_LOWPASS  0
#define EQ_HIGHPASS 1

// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the sample rate
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 48000.00f, 16);

// Create an instance for each EQ block
firstOrderEQ eq1;
firstOrderEQ eq2;

void setup() 
{  
  Serial.begin(115200);
  Serial.println(F("ADSP assignment"));

  Wire.begin(13, 12);
  dsp.begin();

  delay(2000);

  
  Serial.println(F("Pinging i2c lines... 0 = active, 2 = inactive"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp.ping());
 
  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println("Done!\n");

  // Set volume to 0dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, 0);

  edit_eq1(EQ_LOWPASS,  100, 0, 1);
  edit_eq2(EQ_HIGHPASS, 100, 0, 1);
}

void loop() 
{ 
  while (Serial.available() == 0) {}     //wait for data available
  String input = Serial.readString();  //read until timeout
  input.trim();                        // remove any \r \n whitespace at the end of the String
  const char* input_char = input.c_str();

  // String data received
  char eq[10];
  char filter_type[10];
  char freq[10];
  char mute[10];

  sscanf(input_char, "%s %s %s %s", eq, filter_type, freq, mute);

  int valid_string = 1;

  String eq_string = String(eq);
  String filter_type_string = String(filter_type);
  String freq_string = String(freq);
  String mute_string = String(mute);

  int freq_int = freq_string.toInt();

  int filter_type_int = 0;
  if(filter_type_string == "lowpass"){
    filter_type_int = EQ_LOWPASS;
  }
  else if(filter_type_string == "highpass"){
    filter_type_int = EQ_HIGHPASS;  
  }
  else{valid_string = 0;}

  int mute_int = 0;
  if(mute_string == "mute"){
    mute_int = 1;  
  }
  else if(mute_string == "unmute"){
    mute_int = 0;  
  }
  else{valid_string = 0;}

  if(valid_string){
    if(eq_string == "eq1"){
      Serial.println("editing eq1....");
      edit_eq1(filter_type_int, freq_int, 0, mute_int);
    } 
    else if(eq_string == "eq2"){
      Serial.println("editing eq2...."); 
      edit_eq2(filter_type_int, freq_int, 0, mute_int);
    } 
    else{Serial.println("Invalid eq selection.");}
  }
  else{Serial.println("Invalid string");}

  Serial.println(eq);
  Serial.println(filter_type);
  Serial.println(freq_int);
  Serial.println(mute);

  /*
  if (teststr == "channel 1 eq off") {
    Serial.println("Turning off channel 1 eq highpass @200hz");
    edit_eq1(EQ_HIGHPASS,  200, 0, 1);
  } else if (teststr == "channel 1 eq on") {
    Serial.println("Turning on channel 1 eq highpass @200hz");
    edit_eq1(EQ_HIGHPASS,  200, 0, 0);
  }
  */
}

bool edit_eq1(int filterType, int freq, int gain, bool mute)
{
  bool edit_success = true;
  switch(filterType)
  {
    case EQ_LOWPASS:
      eq1.filterType = parameters::filterType::lowpass;
      break;
    case EQ_HIGHPASS:
      eq1.filterType = parameters::filterType::highpass;
      break;
    default:
      edit_success = false;
  }
  eq1.freq = freq;
  eq1.gain = gain;
  if(mute)
  {
    eq1.state = parameters::state::off;
  }
  else
  {
    eq1.state = parameters::state::on;
  }
  dsp.EQfirstOrder(MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR, eq1);
  return edit_success;
}

bool edit_eq2(int filterType, int freq, int gain, bool mute)
{
  bool edit_success = true;
  switch(filterType)
  {
    case EQ_LOWPASS:
      eq2.filterType = parameters::filterType::lowpass;
      break;
    case EQ_HIGHPASS:
      eq2.filterType = parameters::filterType::highpass;
      break;
    default:
      edit_success = false;
  }
  eq2.freq = freq;
  eq2.gain = gain;
  if(mute)
  {
    eq2.state = parameters::state::off;
  }
  else
  {
    eq2.state = parameters::state::on;
  }
  dsp.EQfirstOrder(MOD_GEN1STORDER2_ALG0_PARAMB00_ADDR, eq2);
  return edit_success;
}
