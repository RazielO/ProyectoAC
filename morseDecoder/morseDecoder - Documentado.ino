//Incluimos la libreria del display para poder controlarlo
#include <LiquidCrystal.h>

/**
 * rs = sirve como bandera para indicar de que registro leer Instruction Register (clear, cursor shift) o Data Register (almacena información temporal)
 * en = sirve para indicarle al display que va a recibir datos
 * d4-7 = Conexiones de entrada estas son ocupadas para datos formateados en 4 bits
*/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

//Construimos un objeto con los dandoles la configuración previa con los pines del lcd
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

//La unidad en tiempo que se detectera en alto la señal, es decir, para detectar cuando el foco esta predido
const int UNIT_LENGTH = 100;

//La cadidad de caracteres que puede almacenar el buffer
const int BUFFER_SIZE = 5;

//String que ira almacenando el caracter encontrado para mostrarlo en pantalla
String cadena
/**
 * Este enumerado sirve para configurar una Señal con los siguientes parametros:
 * NOISE : La cantidad de ruido que hay en el ambiente, es decir, que tanta luz hay interfiriendo
 * DAH : Número de "dashes" que tienen por defecto dentro de una señal
 * ELEMENTGAP : dit o dah
 * DIT : Número de "dots" que tienen por defecto que tendra una señal
 * GAP : Espacio entre dits and dahs
 * LONGGAP : Espacio maximo entre señales para determinar el final
*/
enum class Signal: byte {
  NOISE = 0,
  DIT = 1,
  DAH = 2,
  ELEMENTGAP = 3,
  GAP = 4,
  LONGGAP = 5
};

/**
 * Definimos el buffer en una estructura para almacenar más de una
 * señal
*/
struct MorseCodeElement {
  Signal m_signal;

  //Variable de longitud extendedida donde tiene la capacidad de almacenar 32 bits de datos
  unsigned long m_duration;
};

/**
 * Clase que manejará el control sobre el buffer que iran almacenando los dit y dahs que entren
 * m_size : Tamaño del buffer
 * m_head : Inicio del buffer
 * m_tail : Posición actual dentro del buffer
 * m_buffer : Señal Morse que se va a ir almacenando
*/
class MorseCodeBuffer {
  int m_size;
  int m_head;
  int m_tail;
  MorseCodeElement* m_buffer;

/**
 * Constructor de la clase MorseCodeBuffer
*/
public:
  MorseCodeBuffer(int size) {
    // Sirve para distiguir si el buffer esta vacío
    size++;
    
    m_size = size;
    m_head = 0;
    m_tail = 0;

    //Creamos el buffer con la medida del mismo
    m_buffer = new MorseCodeElement[size];
  }

  //Método que sirve para ir almacenando el elemento de Codigo morse en el buffer
  bool Enqueue(MorseCodeElement element) {
    //Verificamos la posición dentro del buffer    
    int new_tail = (m_tail + 1) % m_size;
    
    // Verificamos si el buffer esta lleno
    if (new_tail == m_head) {
      return false;
    }

    //Aumentamos la posición en uno
    m_tail = new_tail;

    //Guardamos el ultimo elemento
    m_buffer[m_tail] = element;
    
    return true;
  }

  //Sacamos un elemento del buffer
  bool TryDequeue(MorseCodeElement* element) {
    // Verfificamos si esta vacío el buffer
    if (m_head == m_tail) {
      return false;
    }

    //sacamos el elemento del buffer
    *element = m_buffer[m_head];

    //Recorremos en una posición el inicio del buffer 
    m_head = (m_head + 1) % m_size;
    return true;
  }

  //Método que regresa la posición actual
  int GetCount() {
      if (m_head == m_tail) {
        return 0;
      }
    
      return (m_tail - m_head + m_size) % m_size;
  }
};

/**
 * Clase que servirá para configurar el sensor de luz en este caso fotoresistor
*/
class AdaptiveLogicLevelProcessor {
  //Cantidad de luz minima que puede recibir el sensor
  int m_sensorMinValue = 1023;

  //Cantidad maxima de luz que puede recibir el sensor
  int m_sensorMaxValue = 0;

  //Cantidad promedio de luz
  int m_sensorMedianValue = 511;

  //Tiempo desde la ultima calibración
  unsigned long m_sensorCalibrationTime = 0;

  //Verificar si el sensor esta o no calibrado
  bool m_calibrated;


public:
  //Definimos el constructor con los valores por defecto
  AdaptiveLogicLevelProcessor() {
    m_sensorMinValue = 1023;
    m_sensorMaxValue = 0;
    m_sensorMedianValue = 511;
    m_sensorCalibrationTime = 0;
  }

  /**
   * Método que sirve para calibrar el sensor
   * @param sensorValue = El valor que se esta recibiendo por el pin de entrada del sensor
   * @digiralInputValue = Valor detectado por el sensor Alto o Bajo
  */
  bool process(int sensorValue, int* digitalInputValue) {
    //Sacamos el tiempo que ha estado ensendido el arduino en milisegundos
    unsigned long currentTime = millis();
  
    // Verificamos si el sensor se ha recalibrado hace 5 segundos, si pasaron más de 5 segundos se vuelve a calibrar
    if (currentTime - m_sensorCalibrationTime > 5000) {
      if (m_sensorMinValue < m_sensorMaxValue) {
        //Si el tiempo entre el maximo y minumo es mayor hay que re calibrar
        if (m_sensorMaxValue - m_sensorMinValue > 20) {
          m_sensorMedianValue = m_sensorMinValue + (m_sensorMaxValue - m_sensorMinValue) / 2;
          m_calibrated = true;
        } else {
          m_calibrated = false;
        }
      }
  
       m_sensorMaxValue = 0;
       m_sensorMinValue = 1023;
       m_sensorCalibrationTime = currentTime;
    }

    if (m_sensorMinValue > sensorValue) {
      m_sensorMinValue = sensorValue;
    }

    if (m_sensorMaxValue < sensorValue) {
      m_sensorMaxValue = sensorValue;
    }
    
    if (!m_calibrated) {
      return false;
    }

    //Escribimos el valor que detecto
    *digitalInputValue = sensorValue > m_sensorMedianValue ? HIGH : LOW;
    return true;
  }
};

/**
 * Clase que decofica las señales que va detectando el sesor
*/
class MorseCodeElementProcessor {
  //Ultima vez que recibio algo en tiempo
  unsigned long m_previousTime = 0;

  //Ultimo valor que recibio
  int m_previousSignal = LOW;

  //Valor maximo y minimo de tiempo para el primer dit o dah
  int m_oneUnitMinValue;
  int m_oneUnitMaxValue;

  ////Valor maximo y minimo de tiempo para un dit o dah del medio de la señal
  int m_threeUnitMinValue;
  int m_threeUnitMaxValue;

  //Valor maximo y minimo de tiempo para el ultimo dit o dah
  int m_sevenUnitMinValue;
  int m_sevenUnitMaxValue;

public:
  /**
   * Sirve para detectar la distancia entre elemetos de la señal 
   * @param tiempo esperado entre señales
  */
  MorseCodeElementProcessor(int unitLengthInMilliseconds) {
    m_oneUnitMinValue = (int)(unitLengthInMilliseconds * 0.5);
    m_oneUnitMaxValue = (int)(unitLengthInMilliseconds * 1.5);
    
    m_threeUnitMinValue = (int)(unitLengthInMilliseconds * 2.0);
    m_threeUnitMaxValue = (int)(unitLengthInMilliseconds * 4.0);

    m_sevenUnitMinValue = (int)(unitLengthInMilliseconds * 5.0);
    m_sevenUnitMaxValue = (int)(unitLengthInMilliseconds * 8.0);
  }

  bool process(int newSignal, MorseCodeElement* element) {
    unsigned long currentTime = millis();
    unsigned long elapsed;

    //Saber si se debe almacenar la señal o no
    bool shouldBuffer = false;

    //Agregamos por defecto que la señal que llego es ruido
    element->m_signal = Signal::NOISE;
    
    // Verificamos que hubo un cambio de bajo a alto
    if (m_previousSignal == LOW && newSignal == HIGH) {
      //Tiempo transcurrido entre señales
      elapsed = currentTime - m_previousTime;

      //Agregar ese timepo a la señal
      element->m_duration = elapsed;

      //Verificamos que si la señal que llego es un conjuto de espacios o un salto de linea
      if (m_sevenUnitMinValue <= elapsed) {
        element->m_signal = Signal::LONGGAP;
        shouldBuffer = true;
        //Verificamos si la señal es un espacio
      } else if (m_threeUnitMinValue <= elapsed && elapsed <= m_threeUnitMaxValue) {
        element->m_signal = Signal::GAP;
        shouldBuffer = true;
        //Verificamos si el valor nuevo entra en el rango de elemento
      } else if (m_oneUnitMinValue <= elapsed && elapsed <= m_oneUnitMaxValue) {
        element->m_signal = Signal::ELEMENTGAP;
        shouldBuffer = true;
      } else {
        //Si no entra en ningun grupo lo detecta como ruido
        element->m_signal = Signal::NOISE;
        shouldBuffer = true;
      }
          
      m_previousSignal = HIGH;
      m_previousTime = currentTime;
      //Detectamos el cambio de alto a bajo
    } else if (m_previousSignal == HIGH && newSignal == LOW) {
      elapsed = currentTime - m_previousTime;
      element->m_duration = elapsed;

      //Verificamos que sea un DAH
      if (m_threeUnitMinValue <= elapsed && elapsed <= m_threeUnitMaxValue) {
        element->m_signal = Signal::DAH;
        shouldBuffer = true;
        //Verificamos que se un DIT
      } else if (m_oneUnitMinValue <= elapsed && elapsed <= m_oneUnitMaxValue) {
        element->m_signal = Signal::DIT;
        shouldBuffer = true;
        //Si no ruido
      } else {
        element->m_signal = Signal::NOISE;
        shouldBuffer = true;
      }
  
      m_previousSignal = LOW;
      m_previousTime = currentTime;  
    }
    
    return shouldBuffer;
  }
};

/**
 * Calse que contiene todas las codificaciones en forma de árbol que servirá 
 * para determinar que número, caracter o letra se a escrito
*/
class MorseCodeProcessor {
  private:  
    //Medida del arbo
    static const int TREE_SIZE = 255;

    //Creamos el árbol en forma de arreglo
    static constexpr char tree[TREE_SIZE] = {
      '\0', '\0', '\0', '5', '\0', '\0', '\0', 'H', '\0', '\0', '\0', '4', '\0', '\0', '\0', 'S',
      '\0', '\0', '$', '\0', '\0', '\0', '\0', 'V', '\0', '\0', '\0', '3', '\0', '\0', '\0', 'I',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'F', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'U',
      '\0', '?', '\0', '\0', '\0', '_', '\0', '\0', '\0', '\0', '\0', '2', '\0', '\0', '\0', 'E',
      '\0', '\0', '\0', '&', '\0', '\0', '\0', 'L', '\0', '"', '\0', '\0', '\0', '\0', '\0', 'R',
      '\0', '\0', '\0', '+', '\0', '.', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'A',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'P', '\0', '@', '\0', '\0', '\0', '\0', '\0', 'W',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'J', '\0', '\'', '\0', '1', '\0', '\0', '\0', '\0',
      '\0', '\0', '\0', '6', '\0', '-', '\0', 'B', '\0', '\0', '\0', '=', '\0', '\0', '\0', 'D',
      '\0', '\0', '\0', '/', '\0', '\0', '\0', 'X', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'N',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'C', '\0', ';', '\0', '\0', '\0', '!', '\0', 'K',
      '\0', '\0', '\0', '(', '\0', ')', '\0', 'Y', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'T',
      '\0', '\0', '\0', '7', '\0', '\0', '\0', 'Z', '\0', '\0', '\0', '\0', '\0', ',', '\0', 'G',
      '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'Q', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'M',
      '\0', ':', '\0', '8', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'O',
      '\0', '\0', '\0', '9', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '0', '\0', '\0', '\0'
    };

    //Bandera que detecta un error
    bool m_error;

    //Incio de la codificación
    int m_start;

    //Final de la codificación
    int m_end;

    //Posición en el árbol
    int m_index;

    //Señal previa recibida
    Signal m_previousInput;

    //Sirve para reiniciar la decodificación
    void reset() {
      m_error = false;
      m_start = 0;
      m_end = TREE_SIZE;
      m_index = (m_end - m_start) / 2;
    }
  
  public:
    MorseCodeProcessor() {
      reset();
      m_previousInput = Signal::NOISE;
    }

    //Buscamos la señal en el árbol
    bool process(Signal input, char* output) {
      bool completed = false;
      
      //Si la señal es un punto comenzamos a buscar en la parte izquierda el árbol
      if (!m_error && input == Signal::DIT) {
        if (m_start == m_index) {
          m_error = true;
        } else {
          m_end = m_index;
          m_index = m_start + (m_end - m_start) / 2;
        }
      //Si es un DAH comenza a buscar en el lado derecho del árbol
      } else if (!m_error && input == Signal::DAH) {
        if (m_end == m_index) {
          m_error = true;
        } else {
          m_start = m_index + 1;
          m_index = m_start + (m_end - m_start) / 2;
        }
      } else if (input == Signal::GAP || input == Signal::LONGGAP) {
        completed = !m_error && tree[m_index] != 0;
        
        if (completed) {
          //Ponemos en la primera posición el caracter que se encontro
          output[0] = tree[m_index];

          //Agrenos un espació a la salida
          output[1] = '\0';

          //si es un salto de linea o conjunto de espacios
          if (input == Signal::LONGGAP) {
            output[1] = ' ';
            output[2] = '\0';
          }
        }
        
        reset();
      }

      m_previousInput = input;

      return completed;
    }
};

//Inicializamos el árbol
constexpr char MorseCodeProcessor::tree[];

//Creamos el buffer con la medida predefinida
MorseCodeBuffer buffer(BUFFER_SIZE);

//Creamos el procesador del Código Morse
MorseCodeProcessor morseCodeProcessor;

//Creamos el calibrador del sensor
AdaptiveLogicLevelProcessor logicLevelProcessor;

//Creamos el dector de señales con la unidad de timepo definido
MorseCodeElementProcessor morseCodeElementProcessor(UNIT_LENGTH);


/************************************************************
 * Se crea una interrupción para procesar la señal de entrada, y sera llamada cada milisegundo
 ************************************************************/
SIGNAL(TIMER0_COMPA_vect) {
  //Activa la interrupción
  cli();

  //Donde se guardará el valor
  int digitalInputValue;

  //Calibramos el sensor en caso de ser necesario y detectamos si es un alto o bajo la señal entrante
  if (logicLevelProcessor.process(analogRead(A0), &digitalInputValue)) {

    //Creamos el Dit o Dah
    MorseCodeElement element;

    //Verificamos que tipo de elemento se detecto
    if (morseCodeElementProcessor.process(digitalInputValue, &element)) {
      //Se almacena en el buffer
      buffer.Enqueue(element);
    }
  }

  //Liberamos la interrupción
  sei();
}

void setup() {
  //Abrimos el puerto serial
  Serial.begin(9600);

  //Activamos la pantalla lcd 
  lcd.begin(16, 2);

  // Creamos un timer para ello usamos una interrupción
  cli();

  //OCRA es el registro del TIMER0 que tiene el arduino 
  OCR0A = 0xAF;

  //TIMSKO registro de interrupciones mascarables o contador, para permitir o no interrupcion
  TIMSK0 |= _BV(OCIE0A);
  
  //liberamos la interrupción
  sei();
}

/************************************************************
 * Una interrupcion para sacar el caracter del buffer de modo seguro
 ************************************************************/
bool TryDequeueSafe(MorseCodeElement* element) {
  // Se hace con una interrupcion para evitar que se altere el buffer mientras se esta sacando algo
  cli();
  bool result = buffer.TryDequeue(element);
  sei();

  return result;
}

//Almacenara el caracter encontrado
char* output = new char[3];

void loop() {
  //Creamos el elemento de la señal
  MorseCodeElement element;
  
  //Sacamos el elemento del buffer
  while (TryDequeueSafe(&element)) {

    //Se encuentra el caracter y se imprime
    if (morseCodeProcessor.process(element.m_signal, output)) {
      cadena = cadena + output;
    }

    //Verificamos que sea un salto de linea o conjunto de espacios, cuando se termino de leer
    if (element.m_signal == Signal::LONGGAP) {
      lcd.clear();
      
      //imprimimos el resultado de la cadena, con lo leido
      Serial.println(cadena);
      lcd.print(cadena);
      cadena = "";
    }
  }

  //Recibimos el resultado mandado por Python
  if(Serial.available()){
    String lectura = Serial.readString();
    lcd.setCursor(0, 1);
    lcd.print(lectura);
  }
}
