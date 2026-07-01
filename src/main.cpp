#include <Audio.h>

// La Teensy est MASTER TDM : BCLK pin 21, FS pin 20, data out 7, data in 8.
#define Osc 0
#define OscTDM2Codec 0
#define OscCodec 0
#define Usb 0
#define SerialUSB 0

#if !Osc || OscTDM2Codec || OscCodec

DMAMEM AudioControlCS42448 cs42448_1;      // Contrôleur matériel CsS42448dd

AudioInputTDM       tdm_codec_in;
AudioOutputTDM       tdm_codec_out;
#endif
#if Osc || OscTDM2Codec || OscCodec
AudioSynthWaveform osc[6];
#endif

AudioOutputTDM2     tdm_daisy_out;       // 16 ports -> 8 slots de 32 bits
AudioInputTDM2      tdm_daisy_in;
AudioAnalyzePeak    peak_daisy;    // ce que la Daisy nous renvoie
AudioAnalyzePeak    peaks[6];      

#if Usb

#define USBIn 1
#define USBOut 1

#if USBIn
AudioInputUSB            usbIn;
#endif
#if USBOut
AudioOutputUSB           usbOut;           // Sortie audio de la teensy
#endif
#endif

AudioMixer4              mixer_1a4;   
AudioMixer4              mixer_5et6;  
AudioMixer4              master;      

// 6 canaux hexa vers la Daisy : ports PAIRS 0..10 = slots 0..5
#if Osc
AudioConnection c0(osc[0], 0, tdm_daisy_out,  0);
AudioConnection c1(osc[1], 0, tdm_daisy_out,  2);
AudioConnection c2(osc[2], 0, tdm_daisy_out,  4);
AudioConnection c3(osc[3], 0, tdm_daisy_out,  6);
AudioConnection c4(osc[4], 0, tdm_daisy_out,  8);
AudioConnection c5(osc[5], 0, tdm_daisy_out, 10);
#elif OscTDM2Codec
AudioConnection c0(osc[0], 0, tdm_daisy_out,  0);
AudioConnection c1(osc[1], 0, tdm_daisy_out,  2);
AudioConnection c2(osc[2], 0, tdm_daisy_out,  4);
AudioConnection c3(osc[3], 0, tdm_daisy_out,  6);
AudioConnection c4(osc[4], 0, tdm_daisy_out,  8);
AudioConnection c5(osc[5], 0, tdm_daisy_out, 10);

AudioConnection mix0(tdm_daisy_in,  0, mixer_1a4 , 0);
AudioConnection mix1(tdm_daisy_in,  2, mixer_1a4 , 1);
AudioConnection mix2(tdm_daisy_in,  4, mixer_1a4 , 2);
AudioConnection mix3(tdm_daisy_in,  6, mixer_1a4 , 3);
AudioConnection mix4(tdm_daisy_in,  8, mixer_5et6, 0);
AudioConnection mix5(tdm_daisy_in, 10, mixer_5et6, 1);
#elif OscCodec
AudioConnection c0(osc[0], 0, mixer_1a4,  0);
AudioConnection c1(osc[1], 0, mixer_1a4,  1);
AudioConnection c2(osc[2], 0, mixer_1a4,  2);
AudioConnection c3(osc[3], 0, mixer_1a4,  3);
AudioConnection c4(osc[4], 0, mixer_5et6, 0);
AudioConnection c5(osc[5], 0, mixer_5et6, 1);
#elif USBIn
AudioConnection c0(usbIn, 0, tdm_daisy_out,  0);
AudioConnection c1(usbIn, 0, tdm_daisy_out,  2);
AudioConnection c2(usbIn, 0, tdm_daisy_out,  4);
AudioConnection c3(usbIn, 1, tdm_daisy_out,  6);
AudioConnection c4(usbIn, 1, tdm_daisy_out,  8);
AudioConnection c5(usbIn, 1, tdm_daisy_out, 10);

AudioConnection mix0(tdm_daisy_in,  0, mixer_1a4 , 0);
AudioConnection mix1(tdm_daisy_in,  2, mixer_1a4 , 1);
AudioConnection mix2(tdm_daisy_in,  4, mixer_1a4 , 2);
AudioConnection mix3(tdm_daisy_in,  6, mixer_1a4 , 3);
AudioConnection mix4(tdm_daisy_in,  8, mixer_5et6, 0);
AudioConnection mix5(tdm_daisy_in, 10, mixer_5et6, 1);
#else 
AudioConnection c0(tdm_codec_in, 10, tdm_daisy_out,  0);
AudioConnection c1(tdm_codec_in,  8, tdm_daisy_out,  2);
AudioConnection c2(tdm_codec_in,  6, tdm_daisy_out,  4);
AudioConnection c3(tdm_codec_in,  4, tdm_daisy_out,  6);
AudioConnection c4(tdm_codec_in,  2, tdm_daisy_out,  8);
AudioConnection c5(tdm_codec_in,  0, tdm_daisy_out, 10);


AudioConnection mix0(tdm_daisy_in,  0, mixer_1a4 , 0);
AudioConnection mix1(tdm_daisy_in,  2, mixer_1a4 , 1);
AudioConnection mix2(tdm_daisy_in,  4, mixer_1a4 , 2);
AudioConnection mix3(tdm_daisy_in,  6, mixer_1a4 , 3);
AudioConnection mix4(tdm_daisy_in,  8, mixer_5et6, 0);
AudioConnection mix5(tdm_daisy_in, 10, mixer_5et6, 1);
#endif

#if Osc || OscTDM2Codec || OscCodec
AudioConnection p0(osc[0], 0, peaks[0], 0);
AudioConnection p1(osc[1], 0, peaks[1], 0);
AudioConnection p2(osc[2], 0, peaks[2], 0);
AudioConnection p3(osc[3], 0, peaks[3], 0);
AudioConnection p4(osc[4], 0, peaks[4], 0);
AudioConnection p5(osc[5], 0, peaks[5], 0);
#else
AudioConnection p0(tdm_codec_in, 10, peaks[0], 0);
AudioConnection p1(tdm_codec_in,  8, peaks[1], 0);
AudioConnection p2(tdm_codec_in,  6, peaks[2], 0);
AudioConnection p3(tdm_codec_in,  4, peaks[3], 0);
AudioConnection p4(tdm_codec_in,  2, peaks[4], 0);
AudioConnection p5(tdm_codec_in,  0, peaks[5], 0);
#endif

AudioConnection mast1(mixer_1a4, 0, master, 0);
AudioConnection mast2(mixer_5et6, 0, master, 1);

#if Usb
#if USBOut
AudioConnection p_outL_usb(master, 0, usbOut, 0);
AudioConnection p_outR_usb(master, 0, usbOut, 1);
#endif
#endif

#if !Osc || OscTDM2Codec || OscCodec
AudioConnection c13(master, 0, tdm_codec_out, 12);
AudioConnection c14(master, 0, tdm_codec_out, 14);
#endif

// Retour : out[6] de la Daisy = slot 6 = port pair 12 
AudioConnection c6(tdm_daisy_in, 12, peak_daisy, 0);

const int reset_p = 34; 

void setup()
{
    AudioMemory(1500);   // TDM exige >= 16 blocs ; 50 = confortable
#if SerialUSB
    Serial.begin(115200);
#endif

#if !Osc || OscTDM2Codec || OscCodec
    pinMode(reset_p, OUTPUT);                                    
    //Power-Up Sequence
    digitalWrite(reset_p, LOW);
    delay(800);
    digitalWrite(reset_p, HIGH);
    delay(10);

    if (cs42448_1.enable()) {
        //Serial.println("configured CS42448");
    } else {
        //Serial.println("failed to config CS42448");
    }
    
    cs42448_1.inputLevel(1);
    cs42448_1.volume(1.0);
#endif
#if Osc || OscTDM2Codec || OscCodec
    for(int i = 0; i < 6; i++)
        osc[i].begin(0.5f, 440.0f, WAVEFORM_SINE); // 110..660 Hz
#endif

#if !SerialUSB
    Serial.end();
#endif
}

void loop()
{

#if SerialUSB
    if (peaks[0].available()) {
        Serial.print("Niveaux des cordes (peak): ");
        for (int i = 0; i < 6; i++) {
            Serial.print("C");
            Serial.print(i + 1);
            Serial.print(": ");
            // read() consomme la valeur et la retourne.
            Serial.print(peaks[i].read(), 3); 
            if (i < 5) {
                Serial.print(" | ");
            }
        }
        Serial.println();
    }

    Serial.println("Loop effectué");
    
    if(peak_daisy.available())
    {
        // Attendu : ~0.2 (amplitude du test tone 440 Hz généré par la Daisy)
        Serial.print("Retour Daisy slot 6, peak = ");
        Serial.println(peak_daisy.read(), 3);
    }
#endif

    //delay(500);
}