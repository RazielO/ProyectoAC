const int PIN = 13;
const int DIT_TIME = 100;

const char* reading = "";

const char* codeset[] = {
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

void setup()
{
    pinMode(PIN, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    reading = readSerial();

    Serial.print(reading);
    Serial.print(" = ");

    for (int i = 0; i < strlen(reading); i++)
    {
      char c = reading[i];

      if(c == ' ')
        space();
      else
      {
        const char* code = getCode(c);

        if(code == NULL)
          continue;
        else
        {
           Serial.print(code);
           Serial.print('/');
           for (int j = 0; j < strlen(code); j++)
              if(code[j] == '.')
                dit();
              else
                dah();
        }
        letter();
      }
    }

    Serial.println();
}

void dit()
{
    digitalWrite(PIN, HIGH);
    delay(DIT_TIME);
    digitalWrite(PIN, LOW);
    delay(DIT_TIME);
}

void dah()
{
    digitalWrite(PIN, HIGH);
    delay(DIT_TIME * 3);
    digitalWrite(PIN, LOW);
    delay(DIT_TIME);
}

void space()
{
    digitalWrite(PIN, LOW);
    delay(DIT_TIME * 9);
}

void letter()
{
    digitalWrite(PIN, LOW);
    delay(DIT_TIME * 3);
}

char* readSerial() 
{
  String retVal = "";
  char c;
  while(!Serial.available())
    delay(10);
  while(Serial.available() > 0) 
  { //Read the serial input
    c = Serial.read();
    if (c != '\n')
      retVal.concat(c);
    delay(10);
  }
  retVal.toLowerCase(); //Convert to lower case
  return strdup(retVal.c_str());
} 

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
