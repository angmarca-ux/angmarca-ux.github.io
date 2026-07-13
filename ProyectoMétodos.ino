#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo Cintura;
Servo Hombro;
Servo Codo;
Servo Pinzas;

int GRADOS_CINTURA = 85;
int GRADOS_HOMBRO = 20;
int GRADOS_CODO = 115;
int GRADOS_PINZAS = 10;

const float L1 = 8.0;  // Longitud del Hombro al Codo (cm)
const float L2 = 14.0; // Longitud del Codo a la Garra (cm)


#define VRX_2 A1
#define VRY_2 A0
#define VRY_1 A2
#define VRX_1 A3

// Pines Digitales para los Botones Internos (SW) del Joystick
#define BOTON_A 2  // SW del Joystick Izquierdo (Muestrea Punto A)
#define BOTON_B 4  // SW del Joystick Derecho (Detiene Muestreo en Punto B)

// Variables de estado del sistema
bool objetoSujetado = false; 

// Variables para el control de tiempo asíncrono
unsigned long tiempoAnterior = 0;
const unsigned long INTERVALO_MUESTREO = 500; // Enviar coordenadas cada 500ms a la PC

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Función para convertir Grados a Radianes
float aRadianes(int grados) {
  return grados * PI / 180.0;
}

// Función trigonométrica de Cinemática Directa para hallar (X, Y, Z) en 3D
void calcularPosicionActual(float &x, float &y, float &z) {
  float radCintura = aRadianes(GRADOS_CINTURA);
  float radHombro  = aRadianes(GRADOS_HOMBRO);
  float radCodo    = aRadianes(GRADOS_HOMBRO + GRADOS_CODO - 90); 

  // Proyección del brazo en el plano horizontal (Radio de alcance)
  float r = L1 * cos(radHombro) + L2 * cos(radCodo);

  // Coordenadas espaciales tridimensionales reales en centímetros
  x = r * cos(radCintura);
  y = r * sin(radCintura);
  z = L1 * sin(radHombro) + L2 * sin(radCodo);
}

void setup() {
  Serial.begin(9600);

  Cintura.attach(8);
  Hombro.attach(6);  
  Codo.attach(10);
  Pinzas.attach(11);

  pinMode(VRY_1, INPUT);
  pinMode(VRX_1, INPUT);
  pinMode(VRY_2, INPUT);
  pinMode(VRX_2, INPUT);

  pinMode(BOTON_A, INPUT_PULLUP);
  pinMode(BOTON_B, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
}

void loop() {
  
  int LVRY_1 = analogRead(VRY_1);
  int LVRX_1 = analogRead(VRX_1);
  int LVRX_2 = analogRead(VRX_2);
  int LVRY_2 = analogRead(VRY_2); 

  bool presionandoA = (digitalRead(BOTON_A) == LOW);
  bool presionandoB = (digitalRead(BOTON_B) == LOW);

  // 1. CONTROL DINÁMICO DE CINTURA
  if (LVRY_1 < 340) GRADOS_CINTURA -= 1;
  else if (LVRY_1 > 680) GRADOS_CINTURA += 1;
  GRADOS_CINTURA = min(175, max(0, GRADOS_CINTURA));

  // 2. CONTROL DINÁMICO DE HOMBRO
  if (LVRX_1 < 340) GRADOS_HOMBRO += 1; 
  else if (LVRX_1 > 680) GRADOS_HOMBRO -= 1;
  GRADOS_HOMBRO = min(150, max(10, GRADOS_HOMBRO)); 

  // 3. CONTROL DINÁMICO DE CODO
  if (LVRX_2 < 340) GRADOS_CODO -= 1;
  else if (LVRX_2 > 680) GRADOS_CODO += 1;
  GRADOS_CODO = min(150, max(0, GRADOS_CODO)); 

  // 4. CONTROL GRADUAL DE LA GARRA
  if (LVRY_2 < 340) GRADOS_PINZAS -= 1;
  else if (LVRY_2 > 680) GRADOS_PINZAS += 1;
  GRADOS_PINZAS = min(58, max(10, GRADOS_PINZAS)); 

  // Cálculo de la cinemática directa tridimensional
  float currentX, currentY, currentZ;
  calcularPosicionActual(currentX, currentY, currentZ);

  // 5. MÁQUINA DE ESTADOS / CONTROL DE EVENTOS
  if (presionandoA && !objetoSujetado) {
    objetoSujetado = true;
    Serial.println("START_A"); 
    
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Agarrando Objeto");
    delay(1000); 
    lcd.clear();
  } 
  else if (presionandoB && objetoSujetado) {
    objetoSujetado = false;
    Serial.println("STOP_B");  
    
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Soltando Objeto");
    delay(2000); 
    lcd.clear();
  }

  // 6. ADQUISICIÓN DE LOGS (Muestreo asíncrono por tiempo)
  if (objetoSujetado) {
    unsigned long tiempoActual = millis(); 
    
    if (tiempoActual - tiempoAnterior >= INTERVALO_MUESTREO) {
      tiempoAnterior = tiempoActual; 

      Serial.print(currentX, 4); Serial.print(",");
      Serial.print(currentY, 4); Serial.print(",");
      Serial.println(currentZ, 4);  
    }

    lcd.setCursor(0, 0); lcd.print("Moviendo Objeto ");
    lcd.setCursor(0, 1); lcd.print("hacia el punto B");
  } else {
    // Telemetría limpia en reposo
    lcd.setCursor(0, 0); 
    lcd.print("C:"); lcd.print(GRADOS_CINTURA); lcd.print("   ");
    lcd.setCursor(8, 0);
    lcd.print("H:"); lcd.print(GRADOS_HOMBRO);  lcd.print("   ");
    
    lcd.setCursor(0, 1); 
    lcd.print("Codo:"); lcd.print(GRADOS_CODO); lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print("G:"); lcd.print(GRADOS_PINZAS); lcd.print("  ");
  }

  // Escritura de pulsos PWM física dentro del loop() [Solución al error de compilación]
  Cintura.write(GRADOS_CINTURA);
  Hombro.write(GRADOS_HOMBRO);
  Codo.write(GRADOS_CODO);
  Pinzas.write(GRADOS_PINZAS);

  delay(40); 
}