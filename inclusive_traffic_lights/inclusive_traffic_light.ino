#include <LiquidCrystal_I2C.h> // Library used to control the LCD display via I2C

// Definition of the pins used in the project
const int BUZZER=4;
const int LED_CARRO_VERDE=5;
const int LED_CARRO_AMARELO=6; 
const int LED_CARRO_VERMELHO=7;
const int LED_PEDESTRE_VERDE=10;
const int LED_PEDESTRE_VERMELHO=11;
const int button=12;

unsigned long ultimoPressionamento; // Stores the time of the last button press
int tempoPedestre = 10; // Total crossing time for pedestrians (in seconds)

// Initializes the LCD display (address 0x27, 16 columns and 2 rows)
LiquidCrystal_I2C lcd(0x27,16,2);

void setup()
{  
  configurarPinos();  // Configure all input and output pins
  iniciarSistema();   // Initialize the LCD and set the initial traffic light state
}

void loop()
{
  // Read the current button state
  int state = digitalRead(button);
  
  // If the button is pressed and at least 5000ms have passed since the last activation
  // this helps prevent multiple triggers caused by button noise (simple debounce)
  if (state == HIGH && (millis() - ultimoPressionamento) > 5000)
  {
    changeLights(); 
  }
}

// Configure all Arduino pins used in the project
void configurarPinos()
{
  pinMode(LED_CARRO_VERMELHO, OUTPUT);
  pinMode(LED_CARRO_AMARELO, OUTPUT);
  pinMode(LED_CARRO_VERDE, OUTPUT);
  pinMode(LED_PEDESTRE_VERMELHO, OUTPUT);
  pinMode(LED_PEDESTRE_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(button, INPUT);
}

// Initialize the LCD and define the initial state of the traffic light
void iniciarSistema()
{
  lcd.begin();  

  // Display message informing that the pedestrian must wait
  printPedestreFechado();

  // Initial state: cars can move and pedestrians must wait
  digitalWrite(LED_CARRO_VERDE, HIGH);
  digitalWrite(LED_PEDESTRE_VERMELHO, HIGH);
}

// Display the default message when the pedestrian signal is closed
void printPedestreFechado()
{  
  lcd.setCursor(0,0);
  lcd.print("Semaforo Fechado");

  lcd.setCursor(4,1);
  lcd.print("Aguarde!");
}

// Controls the entire traffic light transition sequence
void changeLights() 
{

  // Cars receive a warning signal (yellow light)
  digitalWrite(LED_CARRO_VERDE, LOW);  
  digitalWrite(LED_CARRO_AMARELO, HIGH);
  delay(2000);

  // Cars stop completely (red light)
  digitalWrite(LED_CARRO_AMARELO, LOW);  
  digitalWrite(LED_CARRO_VERMELHO, HIGH);
  delay(1000);
  
  // Update LCD informing that pedestrians may cross
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Semaforo Aberto");
  
  // Allow pedestrian crossing
  digitalWrite(LED_PEDESTRE_VERMELHO, LOW);

  // Countdown for pedestrian crossing time
  for (int i = 0; i <= tempoPedestre; i++)
  {
    lcd.setCursor(7,1);
    lcd.print("  "); // clear previous number
    lcd.setCursor(7,1);
    lcd.print(tempoPedestre - i); // display remaining time
    
    digitalWrite(LED_PEDESTRE_VERDE, HIGH);

    // During the first seconds the audio signal is slower
    if (i < 5)     
    {
      tone(BUZZER, 500); 
      delay(500);
    }   
    else 
    {
      // In the final seconds the sound becomes faster
      // to warn pedestrians that the crossing time is ending
      tone(BUZZER, 500);
      delay(200);      
      noTone(BUZZER);
      delay(100);
      tone(BUZZER, 600);
      delay(200); 
    }
      
    digitalWrite(LED_PEDESTRE_VERDE, LOW); // blinking LED effect
    noTone(BUZZER);
    delay(500);
  }
  
  // End pedestrian crossing
  digitalWrite(LED_PEDESTRE_VERDE, LOW);
  digitalWrite(LED_PEDESTRE_VERMELHO, HIGH);

  // Visual and sound warning indicating that the crossing is closing
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Fechando...");
  lcd.setCursor(1,1);
  lcd.print("Nao ultrapasse!");

  tone(BUZZER, 1200);    
  delay(2500);
  noTone(BUZZER);
  
  // Return to the initial message
  lcd.clear();
  printPedestreFechado();
  delay(1000);

  // Restore normal traffic flow
  ultimoPressionamento = millis();
  digitalWrite(LED_CARRO_VERMELHO, LOW);
  digitalWrite(LED_CARRO_VERDE, HIGH);
}
