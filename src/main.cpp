#include <Audio.h>

// La Teensy est MASTER TDM : BCLK pin 21, FS pin 20, data out 7, data in 8.
#define Osc 0
#define OscTDM2Codec 0
#define OscCodec 1
#define Usb 1

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

#if Usb
AudioOutputUSB           usbOut;           // Sortie audio de la teensy
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

AudioConnection  c7(tdm_daisy_in,  0, tdm_codec_out,  0);
AudioConnection  c8(tdm_daisy_in,  2, tdm_codec_out,  2);
AudioConnection  c9(tdm_daisy_in,  4, tdm_codec_out,  4);
AudioConnection c10(tdm_daisy_in,  6, tdm_codec_out,  6);
AudioConnection c11(tdm_daisy_in,  8, tdm_codec_out,  8);
AudioConnection c12(tdm_daisy_in, 10, tdm_codec_out, 10);
#elif OscCodec
AudioConnection c0(osc[0], 0, tdm_daisy_out,  0);
AudioConnection c1(osc[1], 0, tdm_daisy_out,  2);
AudioConnection c2(osc[2], 0, tdm_daisy_out,  4);
AudioConnection c3(osc[3], 0, tdm_daisy_out,  6);
AudioConnection c4(osc[4], 0, tdm_daisy_out,  8);
AudioConnection c5(osc[5], 0, tdm_daisy_out, 10);

AudioConnection  c7(tdm_daisy_in,  0, tdm_codec_out,  0);
AudioConnection  c8(tdm_daisy_in,  2, tdm_codec_out,  2);
AudioConnection  c9(tdm_daisy_in,  4, tdm_codec_out,  4);
AudioConnection c10(tdm_daisy_in,  6, tdm_codec_out,  6);
AudioConnection c11(tdm_daisy_in,  8, tdm_codec_out,  8);
AudioConnection c12(tdm_daisy_in, 10, tdm_codec_out, 10);
#else 
AudioConnection c0(tdm_codec_in, 10, tdm_daisy_out,  0);
AudioConnection c1(tdm_codec_in,  8, tdm_daisy_out,  2);
AudioConnection c2(tdm_codec_in,  6, tdm_daisy_out,  4);
AudioConnection c3(tdm_codec_in,  4, tdm_daisy_out,  6);
AudioConnection c4(tdm_codec_in,  2, tdm_daisy_out,  8);
AudioConnection c5(tdm_codec_in,  0, tdm_daisy_out, 10);

AudioConnection  c7(tdm_daisy_in,  0, tdm_codec_out,  0);
AudioConnection  c8(tdm_daisy_in,  2, tdm_codec_out,  2);
AudioConnection  c9(tdm_daisy_in,  4, tdm_codec_out,  4);
AudioConnection c10(tdm_daisy_in,  6, tdm_codec_out,  6);
AudioConnection c11(tdm_daisy_in,  8, tdm_codec_out,  8);
AudioConnection c12(tdm_daisy_in, 10, tdm_codec_out, 10);
#endif

AudioConnection mix0(tdm_daisy_in,  0, mixer_1a4 , 0);
AudioConnection mix1(tdm_daisy_in,  2, mixer_1a4 , 1);
AudioConnection mix2(tdm_daisy_in,  4, mixer_1a4 , 2);
AudioConnection mix3(tdm_daisy_in,  6, mixer_1a4 , 3);
AudioConnection mix4(tdm_daisy_in,  8, mixer_5et6, 0);
AudioConnection mix5(tdm_daisy_in, 10, mixer_5et6, 1);

AudioConnection mast1(mixer_1a4, 0, master, 0);
AudioConnection mast2(mixer_5et6, 0, master, 1);

#if Usb
AudioConnection p_outL_usb(master, 0, usbOut, 0);
AudioConnection p_outR_usb(master, 0, usbOut, 1);
#endif

#if !Osc || OscTDM2Codec || OscCodec
AudioConnection c13(master, 0, tdm_codec_out, 12);
AudioConnection c14(master, 0, tdm_codec_out, 14);
#endif

// Retour : out[6] de la Daisy = slot 6 = port pair 12
AudioConnection c6(tdm_daisy_in, 12, peak_daisy, 0);

const int reset_p = 29; 

void setup()
{
    AudioMemory(50);   // TDM exige >= 16 blocs ; 50 = confortable
    Serial.begin(115200);

#if !Osc || OscTDM2Codec || OscCodec
    pinMode(reset_p, OUTPUT);                                    
    //Power-Up Sequence
    digitalWrite(reset_p, LOW);
    delay(800);
    digitalWrite(reset_p, HIGH);
    delay(10);

    if (cs42448_1.enable()) {
        Serial.println("configured CS42448");
    } else {
        Serial.println("failed to config CS42448");
    }
    
    cs42448_1.inputLevel(1);
    cs42448_1.volume(1.0);
#endif
#if Osc || OscTDM2Codec || OscCodec
    for(int i = 0; i < 6; i++)
        osc[i].begin(0.5f, 110.0f * (i + 1), WAVEFORM_SINE); // 110..660 Hz
#endif

}

void loop()
{
    if(peak_daisy.available())
    {
        // Attendu : ~0.2 (amplitude du test tone 440 Hz généré par la Daisy)
        Serial.print("Retour Daisy slot 6, peak = ");
        Serial.println(peak_daisy.read(), 3);
    }
    delay(500);
}