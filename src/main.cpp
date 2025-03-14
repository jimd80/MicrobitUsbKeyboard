// Settings
#define DEBUG 0                  // 1 = less logging to not affect performance, 2 = extra logging, less performance
#define PLAYER_DELAY_MS 60       // when the same key is received within this delay, don't send repeated key as long as subsequent keys are below this delay (prevent spamming, cheating and backlog floods)
#define KEYPRESS_MAX_TIME_MS 50  // nominal time between the keydown-keyup event. Must be smaller than PLAYER_DELAY_MS to allow time between keyup and the next keydown.
#define KEYPRESS_MIN_TIME_MS 20  // when MAX_KEYS_SAME_TIME is reached, the keypress time can be shortened to make place for the new key. New key presses are discarded (KEYPRESS_BUFFER 0) or delayed (KEYPRESS_BUFFER 1) to ensure KEYPRESS_MIN_TIME_MS
#define KEYPRESS_BUFFER 1        // 1 = do not discard new keypresses when the max capacity for MAX_KEYS_SAME_TIME is reached but send delayed. Important to keep games like flappy bird running when everyone presses at about the same time
#define MAX_KEYS_SAME_TIME 6     // maximum keys pressed at the same time (USB Limit). Max rate = 1000 * MAX_KEYS_SAME_TIME / KEYPRESS_MIN_TIME_MS keys/sec
#define KB_LAYOUT KeyboardLayout_fr_FR

#define FROM_KEY 32
#define TILL_KEY_EXCL 128

// TODO LED's and buttons on interface circuit board
#define LED_RED 8   // atmel PB4  Indicates when paused (supress keystrokes)
#define LED_GREEN 9 // atmel PB5  Show receiving data
#define BUTTON1 18  // atmel PF7  Send single char 'a' to keyboard (to test qwerty / azerty)
#define BUTTON2 19  // atmel PF6  Send 30 keys/sec a..z (2x each)
#define BUTTON3 20  // atmel PF5  Send 300 keys/sec a..z (20x each)
#define BUTTON4 21  // atmel PF4  Pause / Resume

// Includes
#include <Arduino.h>
#include <Keyboard.h>

// Global Variables
bool First = true;
bool Paused = false;
unsigned long LastKbCharSendTimeMs = 0; // last millis() a char was sent to the keyboard, to wait for the next millisecond (send max 1000 events / sec)
unsigned long LastSendTimePerKey[TILL_KEY_EXCL - FROM_KEY]; // last time when this key was sent to the computer
unsigned long LastReceiveTimePerKey[TILL_KEY_EXCL - FROM_KEY]; // last time when this key was received from serial
bool IsKeyPressed[TILL_KEY_EXCL - FROM_KEY]; // flags indicating for which keys a keydown event was sentare times a character was supressed
unsigned char NrKeysDown = 0;

#if DEBUG > 0
    // only 2kb ram, save space for non debug to have more stack space
    unsigned long NrSendPerKey[TILL_KEY_EXCL - FROM_KEY]; // times a character was sent to the kb
    unsigned long NrSupressedPerKey[TILL_KEY_EXCL - FROM_KEY]; // times a character was supressed

    unsigned long ShortestKeypress; // for debug
    unsigned char MaxKeysDown; // for debug
#endif

#if KEYPRESS_MAX_TIME_MS >= PLAYER_DELAY_MS
    #error Illegal values
#endif

#if KEYPRESS_MIN_TIME_MS > KEYPRESS_MAX_TIME_MS
    #error Illegal values
#endif

#if DEBUG > 0
    #warning Debug mode. Avoid using this in real time
#endif

template <class T> void LogNumber(T value, unsigned char digits = 0, char multiplier = 0)
{
    unsigned char revbuf[10];
    unsigned char ctr = 0;

    // avoid printing 0000
    if (multiplier > 0 && value == 0)
        multiplier = 0;

    // make sure digits at at least 1 larger as #decimals (example 0.000)
    if (digits <= -multiplier)
        digits = 1 - multiplier;

    // store number with minimum nr of digits and at least 1 digit
    while (value || !ctr)
    {
        revbuf[ctr++] = (value % 10) + '0';
        value /= 10;
        if (digits > 0)
          digits--;
    }

    // at this point: digits = remaining leading zeros, ctr = actual digits in buffer
    char decimalPos = ctr + digits + multiplier; // position to write the decimal point (# digits before)

    while (digits--)
    {
        if (decimalPos-- == 0)
            Serial.write('.');

        Serial.write('0');
    }

    while (ctr)
    {
        if (decimalPos-- == 0)
            Serial.write('.');

        Serial.write(revbuf[--ctr]);
    }

    while (decimalPos-- > 0)
        Serial.write('0');
}

void LogNumber(long value, unsigned char digits = 0, char multiplier = 0)
{
    if (value < 0)
    {
        Serial.write('-');
        value = -value;
    }

    LogNumber<unsigned long>(value, digits, multiplier);
}

void LogNumber(unsigned long value, unsigned char digits = 0, char multiplier = 0)
{
    LogNumber<unsigned long>(value, digits, multiplier);
}

void LogPStr(const char* pstr)
{
    while (char c = pgm_read_byte(pstr++))
        Serial.write(c);
}

void SetLedR(bool state)
{
    digitalWrite(LED_RED, state ? 1 : 0);
}

void SetLedG(bool state)
{
    digitalWrite(LED_GREEN, state ? 1 : 0);
}

void ClearTimers()
{
    LastKbCharSendTimeMs = 0;
    for (unsigned char i = 0; i < (TILL_KEY_EXCL - FROM_KEY); i++)
    {
        LastReceiveTimePerKey[i] = 0;
        LastSendTimePerKey[i] = 0;

        #if DEBUG > 0
            NrSendPerKey[i] = 0;
            NrSupressedPerKey[i] = 0;
        #endif
    }
    #if DEBUG > 0
        MaxKeysDown = 0;
        ShortestKeypress = KEYPRESS_MAX_TIME_MS;
    #endif
}

// check all keys and release if necessary. If fast is true, allow shorter time than KEYPRESS_MAX_TIME_MS but not shorter than KEYPRESS_MIN_TIME_MS
void ReleaseKeys(bool fast)
{  
    unsigned long currentTime = millis();
    unsigned long longestKey = 0;
    unsigned char longestKeyIndex = 0;

    for (unsigned char i = 0; i < (TILL_KEY_EXCL - FROM_KEY); i++)
    {
        if (IsKeyPressed[i])
        {
            unsigned long pressTime = currentTime - LastSendTimePerKey[i];
            if (pressTime >= KEYPRESS_MAX_TIME_MS)
            {
                // Release key
                Keyboard.release(i + FROM_KEY);
                SetLedG(false);
                IsKeyPressed[i] = false;
                NrKeysDown--;
                return; // continue on the next main loop iteration to increase respond time
            }

            // find the longest pressed key for later use
            if (pressTime > longestKey)
            {
                longestKey = pressTime;
                longestKeyIndex = i;
            }
        }
    }

    if (fast && NrKeysDown >= MAX_KEYS_SAME_TIME && longestKey >= KEYPRESS_MIN_TIME_MS)
    {
        // queue is full, allow to release the longest pressed key
        Keyboard.release(longestKeyIndex + FROM_KEY);
        IsKeyPressed[longestKeyIndex] = false;
        NrKeysDown--;

        #if DEBUG > 0
            if (ShortestKeypress > longestKey)
                ShortestKeypress = longestKey;
        #endif
    }

    if (Paused)
    {
        if (currentTime - LastKbCharSendTimeMs > 20)
            SetLedG(false);
    }
}

void ProcessChar(char c)
{
    // Check valid char
    if (c < FROM_KEY || c >= TILL_KEY_EXCL)
    {
        #if DEBUG >= 1
            LogPStr(PSTR("Unsupported char received\r\n"));
        #endif

        return;
    }

    unsigned char charIndex = c - FROM_KEY;

    // Check time between keys > 1ms
    unsigned long currentTime = millis();

    while (true)
    {
        if (currentTime <= LastKbCharSendTimeMs)
        {
            if (currentTime < LastKbCharSendTimeMs)
            {
                #if DEBUG >= 1
                    LogPStr(PSTR("millis{} overflow, reset timers\r\n"));
                #endif
                ClearTimers();
            }
            currentTime = millis();
            continue; // delay at least 1ms to not send more than 1000 keys/sec to computer
        }
        break;
    }

    LastKbCharSendTimeMs = currentTime;

    // Check time between same key. As an anti-cheat meausure, extend the wait time if the same key is received below a reasonable "minimum human" time.
    unsigned long prevKeyTime = LastReceiveTimePerKey[charIndex];
    LastReceiveTimePerKey[charIndex] = currentTime;

    if ((prevKeyTime + PLAYER_DELAY_MS) > currentTime)
    {
        #if DEBUG > 0
            NrSupressedPerKey[charIndex]++;
        #endif

        #if DEBUG >= 2
            LogPStr(PSTR("Supressed repeated key\r\n"));
        #endif
        return;
    }

    if (IsKeyPressed[charIndex])
    {
        #if DEBUG >= 1
            LogPStr(PSTR("Error Key is already pressed, should not happen!\r\n"));
        #endif
        return;
    }

    while (NrKeysDown >= MAX_KEYS_SAME_TIME)
    {
        // maximum keys already pressed. try to release a key earlier
        ReleaseKeys(true);

        if (NrKeysDown >= MAX_KEYS_SAME_TIME)
        {
            // unable to free key
            #if KEYPRESS_BUFFER != 1
                // do not buffer if release failed
                #if DEBUG > 0
                    NrSupressedPerKey[charIndex]++;
                #endif

                #if DEBUG >= 2
                    LogPStr(PSTR("Maximum keys pressed, discard key\r\n"));
                #endif
                return;
            #else
                // keep keypresses in serial buffer and wait
                SetLedR(!Paused); // blink to indicate overflow
                delay(1);
                SetLedR(Paused);
            #endif
        }
    }

    SetLedG(true);

    if (Paused)
        return;

    // Send keydown event
    Keyboard.press(c);
    LastSendTimePerKey[charIndex] = currentTime; // ReleaseKeys() needs this to release key after KEYPRESS_TIME_MS
    IsKeyPressed[charIndex] = true;
    NrKeysDown++;

    #if DEBUG > 0
        if (NrKeysDown > MaxKeysDown)
            MaxKeysDown = NrKeysDown;

        NrSendPerKey[charIndex]++;
    #endif
}

void SpeedTest(int nrKeys, unsigned char nrDiffKeys, int delayMs)
{
    unsigned char keyIndex = 0;
    for (int i = 0; i < nrKeys; i++)
    {
        ProcessChar('a' + keyIndex);
        keyIndex = (keyIndex + 1) % nrDiffKeys;
        for (int d = 0; d < delayMs; d++)
        {
            delay(1);
            ReleaseKeys(false);
        }
    }
}

void ProcessButtons()
{
    bool button1Pressed = digitalRead(BUTTON1) == LOW;
    bool button2Pressed = digitalRead(BUTTON2) == LOW;
    bool button3Pressed = digitalRead(BUTTON3) == LOW;
    bool button4Pressed = digitalRead(BUTTON4) == LOW;

    if (button1Pressed)
        ProcessChar('a');

    if (button2Pressed)
        SpeedTest(52, 26, 30); // 30 keys/sec

    if (button3Pressed)
        SpeedTest(520, 26, 3); // 300 keys/sec

    if (button4Pressed)
    {
        Paused = !Paused;
        SetLedR(Paused);
        delay(50); // bounce reduce
        while (digitalRead(BUTTON4) == LOW)
            ;
        delay(50); // bounce reduce
    }
}

void setup()
{
    // Init variables
    First = true;
    Paused = false;
    NrKeysDown = 0;
    for (unsigned char i = 0; i < (TILL_KEY_EXCL - FROM_KEY); i++)
        IsKeyPressed[i] = false;

    ClearTimers();

    // Init hardware
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    pinMode(BUTTON4, INPUT_PULLUP);

    SetLedR(true); // indicate startup (1 sec)
    SetLedG(true);

    #if DEBUG >= 1
        Serial.begin(115200);
        while (!Serial)
        ;
    #endif

    Serial1.begin(115200);
    while (!Serial1)
        ;

    Keyboard.begin(KB_LAYOUT);
}

void loop()
{
    if (First)
    {
        // Startup delay 1 sec
        First = false;
        delay(1000);
        SetLedR(Paused);
        SetLedG(false);

        #if DEBUG >= 1
            LogPStr(PSTR("Serial to HID Ready!\r\n"));
            LogPStr(PSTR("In DEBUG Mode "));
            Serial.write('0' + DEBUG);
            LogPStr(PSTR(". Using anti-spam delay of "));
            LogNumber(PLAYER_DELAY_MS);
            LogPStr(PSTR("ms and a keypress time between "));
            LogNumber(KEYPRESS_MIN_TIME_MS);
            LogPStr(PSTR("ms and "));
            LogNumber(KEYPRESS_MAX_TIME_MS);
            LogPStr(PSTR("ms.\r\n"));
        #endif
    }

    #if DEBUG >= 1
        // Debug
        if (Serial.available())
        {
        char cmd = Serial.read();
        if (cmd == '?')
        {
            LogPStr(PSTR("\r\nStats (c=clear):\r\n"
                "Shortest keypress: "));
            LogNumber(ShortestKeypress);
            LogPStr(PSTR("ms\r\n"
                "Max keys down: "));
            LogNumber(MaxKeysDown);
            LogPStr(PSTR(" (USB limit=6)\r\n"));

            for (unsigned char i = 0; i < (TILL_KEY_EXCL - FROM_KEY); i++)
            {
                if (LastReceiveTimePerKey[i] == 0)
                    continue;

                LogPStr(PSTR("Key '"));
                Serial.write(i + FROM_KEY);
                LogPStr(PSTR(": Send: "));
                LogNumber(NrSendPerKey[i]);
                LogPStr(PSTR(": Supr: "));
                LogNumber(NrSupressedPerKey[i]);
                LogPStr(PSTR("\r\n"));
            }
            LogPStr(PSTR("------\r\nPress c to clear\r\n"));
        }
        else if (cmd == 'c')
        {
            ClearTimers();
            LogPStr(PSTR("Stats cleared.\r\n"));
        }
        else
        {
            //delay(100);
            //Keyboard.write(cmd);
        }
        }
    #endif

    if (Serial1.available())
    {
        #if DEBUG >= 2
            LogPStr(PSTR("Received char at "));
            LogNumber(millis());
            LogPStr(PSTR(" ms.\r\n"));
        #endif

        ProcessChar(Serial1.read());
    }

    ReleaseKeys(false); //check all keys and release if necessary
    ProcessButtons();
}