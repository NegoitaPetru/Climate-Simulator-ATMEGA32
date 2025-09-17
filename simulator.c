#include <LiquidCrystal_I2C.h>

// PWM enable macro helpers
#define ENABLE_PWM_RED()    (TCCR1A |=  (1 << COM1A1))
#define DISABLE_PWM_RED()   (TCCR1A &= ~(1 << COM1A1))
#define ENABLE_PWM_BLUE()   (TCCR1A |=  (1 << COM1B1))
#define DISABLE_PWM_BLUE()  (TCCR1A &= ~(1 << COM1B1))

#define BLUE_BUTTON_PIN PC2
#define RED_LED_PIN PB1
#define BLUE_LED_PIN PB2

LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x28, 16, 2);

uint16_t channel=0;
int32_t Tt,Tm;
uint16_t toggleState=0;
uint16_t lastButtonState=1;

void setup() {

  Serial.begin(9600);

  lcd.begin(16,2);
  lcd2.begin(16,2);
  
  // config. the ADC
  ADMUX = (1<<REFS0) | (0<<MUX0) | (0<<MUX1) | (0<<MUX2) | (0<<MUX3); // potentiometrul A0
  ADCSRA |= (1<<ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

  // config. blue button on PORTC as INPUT
  DDRC &= ~(1<<BLUE_BUTTON_PIN);
  PORTC |= (1<<BLUE_BUTTON_PIN);

  // config PORTB output (for LEDs)
  DDRB |= (1<<RED_LED_PIN) | (1<<BLUE_LED_PIN);


  // config. registers TCCR1A & TCCR1B for PWM
  TCCR1A |= (0 << WGM11) | (1 << WGM10) | (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (0 << COM1B0);
  TCCR1B |= (0 << WGM13) | (1 << WGM12) | (1 << CS11);

  test_convertADCToTemp();

}

void loop() {
  
  if(!(ADCSRA & (1<<ADSC))) // ADC liber
  {
    if(channel==0)
    {
      ADMUX = (1<<REFS0) | (1<<MUX0) | (0<<MUX1) | (0<<MUX2) | (0<<MUX3); // potentiometru A1
      ADCSRA |= (1<<ADSC);
      while (ADCSRA & (1<<ADSC));  // asteptam sa se termine conversia
      int32_t adc = ADC;
      Tm = convertADCToTemp(adc);
      displayTemperature();
      channel = 1;
    }
    else
    {
      ADMUX = (1<<REFS0) | (0<<MUX0) | (0<<MUX1) | (0<<MUX2) | (0<<MUX3); // potentiometru A0
      ADCSRA |= (1<<ADSC);
      while (ADCSRA & (1<<ADSC));  // asteptam sa se termine conversia
      int32_t adc = ADC;
      Tt = convertADCToTemp(adc);
      //Serial.println(Tt);
      displayTemperature();
      channel = 0;
    }
  }

}

void displayTemperature()
{
  uint16_t buttonState = (PINC & (1<<BLUE_BUTTON_PIN));
  if(!buttonState && lastButtonState)
  {
      delay(50);
      ButtonPressed();
      toggleState = ! toggleState;

      lcd.clear();
      lcd.setCursor(0,0);
      if(toggleState == 0)
      {
        lcd.print("Temperatura");
        lcd.setCursor(0,1);
        lcd.print("mediului: ");
        lcd.print(Tm);
        lcd.write(223);
        lcd.print("C.");
      }
      else
      {
        lcd.print("Temperatura");
        lcd.setCursor(0,1);
        lcd.print("tinta: ");
        lcd.print(Tt);
        lcd.write(223);
        lcd.print("C.");
      }    
    testingValues();
    displayOperation();
  }
  lastButtonState = buttonState;
}
void displayOperation()
{
  int32_t difference = abs(Tm - Tt);
  Serial.print("|Tm - Tt| = ");
  Serial.println(difference);
  lcd2.clear();

  // Activam mereu PWM pt ambele canale
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

  if (Tm < Tt)
  {
    lcd2.setCursor(6,0);
    lcd2.print("HEAT");
    lcd2.setCursor(5,1);
    lcd2.print("Tm < Tt");

    if(difference >= 15)
    {
      OCR1A = 255;
      OCR1B = 0;
      ENABLE_PWM_RED();
      DISABLE_PWM_BLUE(); // oprește complet albastrul
    }
    else if(difference >= 10)
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 191;
      OCR1B = 64;
    }
    else if(difference >= 5)
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 127;
      OCR1B = 127;
    }
    else
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 64;
      OCR1B = 191;
    }
  }
  else if (Tm == Tt)
  {
    lcd2.setCursor(6,0);
    lcd2.print("IDLE");
    lcd2.setCursor(5,1);
    lcd2.print("Tm = Tt");

    OCR1A = 0;
    OCR1B = 0;
    DISABLE_PWM_BLUE();
    DISABLE_PWM_RED();

  }
  else
  {
    lcd2.setCursor(6,0);
    lcd2.print("COOL");
    lcd2.setCursor(5,1);
    lcd2.print("Tm > Tt");

    if(difference >= 15)
    {

      OCR1A = 0;    // rosu oprit
      OCR1B = 255;  // albastru maxim

      DISABLE_PWM_RED();
      ENABLE_PWM_BLUE();
    }
    else if(difference >= 10)
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 64;
      OCR1B = 191;
    }
    else if(difference >= 5)
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 127;
      OCR1B = 127;
    }
    else
    {
      ENABLE_PWM_RED();
      ENABLE_PWM_BLUE();
      OCR1A = 191;
      OCR1B = 64;
    }
  }
}


int32_t convertADCToTemp(int32_t adc)
{
  int32_t temperature = 0;
  if( adc <= 320 ) // values of ADC partitioned to be negative temperatures [-40, 0]
  {
    temperature = -40 + (adc / 8);
  }
  else
  {
    temperature = (adc - 320) / 8;
  }
  return temperature;
}

void test_convertADCToTemp() {
  Serial.println("Testing convertADCToTemp:");
  int adcVals[] = {0, 160, 320, 400, 640, 1023};
  for(int i = 0; i < 6; i++) {
    int val = adcVals[i];
    Serial.print("ADC = ");
    Serial.print(val);
    Serial.print(" -> Temp = ");
    Serial.println(convertADCToTemp(val));
  }
}
void ButtonPressed(){
    if((PINC & (1<<BLUE_BUTTON_PIN)) == 0)
    {
      Serial.println("Buton APASAT");
      delay(50);
    }
}
void testingValues(){
  Serial.print("Tm: ");
  Serial.print(Tm);
  Serial.print(" | Tt: ");
  Serial.println(Tt);
}
