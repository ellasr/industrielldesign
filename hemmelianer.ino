#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//brytere til lianer og tilhørende lianer (LED-strips)
#define KNAPP_1 2
#define LED_1   3
#define KNAPP_2 4
#define LED_2   5
#define KNAPP_3 6
#define LED_3   7
#define KNAPP_4 8
#define LED_4   9
#define KNAPP_5 10
#define LED_5   11
#define KNAPP_6 12
#define LED_6   13
#define KNAPP_7 14
#define LED_7   15
#define KNAPP_8 16
#define LED_8   17

//opptaksknapp
#define RECORD_KNAPP 18

//Enkeltstående led som lyser ved siden av opptaksknapp
#define ROD_LED 19

#define LED_ANTALL 60

//Samler knapper i en liste for å spare plass lenger ned i koden:
char knapper[] = {KNAPP_1, KNAPP_2, KNAPP_3, KNAPP_4, KNAPP_5, KNAPP_6, KNAPP_7, KNAPP_8}; 

//Gjør klar stripene:
Adafruit_NeoPixel strip_1(LED_ANTALL, LED_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_2(LED_ANTALL, LED_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_3(LED_ANTALL, LED_3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_4(LED_ANTALL, LED_4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_5(LED_ANTALL, LED_5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_6(LED_ANTALL, LED_6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_7(LED_ANTALL, LED_7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_8(LED_ANTALL, LED_8, NEO_GRB + NEO_KHZ800);

//Samler stripene i en liste for å gjøre det lettere å loope gjennom dem:
Adafruit_NeoPixel strips[] = {strip_1, strip_2, strip_3, strip_4, strip_5, strip_6, strip_7, strip_8};

void setup() { 
  Serial.begin(9600);

  //Gjør klar stripene:
  for(int i = 0; i < 8; i++) {
    strips[i].begin();           // Initierer NeoPixel strip 
    strips[i].setBrightness(50); // Setter lysstyrke til ca 1/5 (max = 255)
    }

  //Setter alle brytere til lianer til INPUT_PULLUP
  for(int i = 0; i < 8; i++) {
    pinMode(knapper[i], INPUT_PULLUP); 
    }

  //Setter record-knappen til INPUT_PULLUP
  pinMode(RECORD_KNAPP, INPUT_PULLUP);

  //Setter den rode ledden til opptakeren til OUTPUT
  pinMode(ROD_LED, OUTPUT);
 
  //Setter alle til å lyse enfarget: (funksjonen er definert lenger ned)
  en_farge();
  
}

void loop() {
  //Løkke for å sjekke alle mikrobrytere til lianene:
  for(int i = 0; i < 8; i++) {
    //sjekker om bryteren trykkes ned: (omvendt logikk med ! pga. INPUT_PULLUP)
    if(!digitalRead(knapper[i])) {
      Serial.print("start"); //sender beskjed til Raspberry Pi om å spille lyd
       
      rainbow(0.3, i); //regnbueeffekt (funksjonen definert lenger ned)            

      //Setter lianen til å lyse ensfarget igjen etter regnbuen er ferdig: 
      for(int j = 0; j < LED_ANTALL; j++) { 
        strips[i].setPixelColor(j, 0, 255, 255);
        strips[i].show();
        }
    }
  }

  //Sjekker om record-knappen trykkes ned:
  if(!digitalRead(RECORD_KNAPP)) {
    Serial.print("record"); //sender beskjed til Raspberry Pi om å ta opp lydklipp
    
    delay(500); //litt delay for å samkjøre med delay på Raspberry Pi (for at folk ikke skal begynne å snakke for fort)
    digitalWrite(ROD_LED, HIGH); //Lys skrus på
    
    delay(10000); //delay for at ikke det skal skje andre ting mens vi tar opp lydklipp

    digitalWrite(ROD_LED, LOW); //Lys skrus av
    }
}

//Funksjon for å kjøre regnbuelys i lianene. i er lianenummer og wait er hastighet på effekten
void rainbow(float wait, int i) { 
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int j=0; j < strips[i].numPixels(); j++) { 
      int pixelHue = firstPixelHue + (j * 65536L / strips[i].numPixels());
      strips[i].setPixelColor(j, strips[i].gamma32(strips[i].ColorHSV(pixelHue)));
    }
    strips[i].show(); 
    delay(wait);  // Pauser en kort stund
  }
}

//Funksjon som setter alle stripene til å lyse én farge:
void en_farge() {
  //en for-løkke for å gå gjennom alle ledsene:
  for(int i = 0; i < 8; i++) {
    //en til for-løkke for å gå gjennom alle pikslene:
    for(int j = 0; j < LED_ANTALL; j++) {
      strips[i].setPixelColor(j, 0, 255, 255);
      strips[i].show();
      }
    }
  }
