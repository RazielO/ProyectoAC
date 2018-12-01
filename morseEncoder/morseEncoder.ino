const int PIN = 13; // Pin en el que se encuentra el LED
const int DIT_TIME = 100; // Valor en milisegundos que tarda un punto

const char* reading = ""; // Lectura del puerto serial

const char* codeset[] = { // Diccionario para obtener el código morse de un caracter
  /* ! */ "-.-.--",
  /* " */ ".-..-.",
  /* # */ NULL,
  /* $ */ "...-..-",
  /* % */ NULL,
  /* & */ ".-...",
  /* ' */ ".----.",
  /* ( */ "-.--.",
  /* ) */ "-.--.-",
  /* * */ NULL,
  /* + */ ".-.-.",
  /* , */ "--..--",
  /* - */ "-....-",
  /* . */ ".-.-.-",
  /* / */ "-..-.",
  /* 0 */ "-----",
  /* 1 */ ".----",
  /* 2 */ "..---",
  /* 3 */ "...--",
  /* 4 */ "....-",
  /* 5 */ ".....",
  /* 6 */ "-....",
  /* 7 */ "--...",
  /* 8 */ "---..",
  /* 9 */ "----.",
  /* : */ "---...",
  /* ; */ "-.-.-.",
  /* < */ NULL,
  /* = */ "-...-",
  /* > */ NULL,
  /* ? */ "..--..",
  /* @ */ ".--.-.",
  /* A */ ".-",
  /* B */ "-...",
  /* C */ "-.-.",
  /* D */ "-..",
  /* E */ ".",
  /* F */ "..-.",
  /* G */ "--.",
  /* H */ "....",
  /* I */ "..",
  /* J */ ".---",
  /* K */ "-.-",
  /* L */ ".-..",
  /* M */ "--",
  /* N */ "-.",
  /* O */ "---",
  /* P */ ".--.",
  /* Q */ "--.-",
  /* R */ ".-.",
  /* S */ "...",
  /* T */ "-",
  /* U */ "..-",
  /* V */ "...-",
  /* W */ ".--",
  /* X */ "-..-",
  /* Y */ "-.--",
  /* Z */ "--..",
  /* [ */ NULL,
  /* \ */ NULL,
  /* ] */ NULL,
  /* ^ */ NULL,
  /* _ */ "..--.-"
 };

/***************************************************/
/* Método para hacer las configuraciones iniciales */
/***************************************************/
void setup()
{
    pinMode(PIN, OUTPUT); // El pin del LED se coloca en modo escritura
    Serial.begin(9600); // Se inicia la conexión con el puerto serial a 9600 baudios
}

/**************************************************************/
/* Método que siempre se está ejecutando en un ciclo infinito */
/**************************************************************/
void loop()
{
    reading = readSerial(); // Se llama un método para leer del puerto serial

    Serial.print(reading); // Se escribe en el puerto serial la lectura
    Serial.print(" = "); // Se escribe en el puerto serial un '='

    for (int i = 0; i < strlen(reading); i++) // Se itera por la cadena
    {
      char c = reading[i]; // Caracter actual de la iteración

      if(c == ' ') // Si es un espacio
        space(); // LLama el metodo para enviar un espacio
      else // Si no
      {
        const char* code = getCode(c); // Se llama el método para obtener el código del caracter

        if(code == NULL) // Si no tiene código
          continue; // Siguiente caracter
        else // Si tiene código
        {
           Serial.print(code); // Se escribe en el puerto serial el código del caracter
           Serial.print('/'); // Se separan los caracteres con un '/'
           for (int j = 0; j < strlen(code); j++) // Se itera por el código
              if(code[j] == '.') // Si es un punto
                dit(); // Método para 'imprimir' un punto en el LED
              else // Si es una raya
                dah(); // Método para 'imprimir' una raya en el LED
        }
        letter(); // Método para separar letras
      }
    }

    Serial.println(); // Se escribe en el puerto serial un salto de línea
}

/**********************************************/
/* Método que prende el led 100 ms y lo apaga */
/* representa un punto en código morse        */
/**********************************************/
void dit()
{
    digitalWrite(PIN, HIGH);
    delay(DIT_TIME);
    digitalWrite(PIN, LOW);
    delay(DIT_TIME);
}

/**********************************************/
/* Método que prende el led 300 ms y lo apaga */
/* representa una raya en código morse        */
/**********************************************/
void dah()
{
    digitalWrite(PIN, HIGH);
    delay(DIT_TIME * 3);
    digitalWrite(PIN, LOW);
    delay(DIT_TIME);
}

/**********************************************/
/* Método que prende el led 400 ms y lo apaga */
/* representa un espacio                      */
/**********************************************/
void space()
{
    digitalWrite(PIN, LOW);
    delay(DIT_TIME * 4);
}

/**********************************************/
/* Método que prende el led 200 ms y lo apaga */
/* representa una separación entre letras     */
/**********************************************/
void letter()
{
    digitalWrite(PIN, LOW);
    delay(DIT_TIME * 2);
}

/**********************************************/
/* Método que lee del puerto serial hasta que */
/* encuentra un salto de línea                */
/**********************************************/
char* readSerial() 
{
  String retVal = ""; // Cadena que se va a regresar
  char c; // Caracter a leer 
  while(!Serial.available()) // Esperar hasta que encuentre algo
    delay(10); // Esperar 10 ms
  while(Serial.available() > 0) // Si hay algo para leer
  { 
    c = Serial.read(); // Se lee el siguiente caracter
    if (c != '\n') // Si no es un salto de línea
      retVal.concat(c); // Agrega el caracter a la cadena
    delay(10); // Esperar 10 ms
  }
  retVal.toLowerCase(); // Convertir a letras minusculas
  return strdup(retVal.c_str()); // Regresa una copia de la cadena que se obtuvo del puerto serial
} 

/***********************************************/
/* Método que obtiene el código morse a partir */
/* del diccionario definido anteriormente      */
/***********************************************/
const char* getCode(char c)
{
    if ('a' <= c && c <= 'z') {
    c = c - ('a' - 'A');
  }

  if ('!' <= c && c <= '_') {
    return codeset[c - '!'];
  }

  return NULL;
}
