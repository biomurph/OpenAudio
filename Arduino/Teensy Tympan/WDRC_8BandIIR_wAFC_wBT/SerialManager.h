
#ifndef _SerialManager_h
#define _SerialManager_h

#include <Tympan_Library.h>

//add in the algorithm whose gains we wish to set via this SerialManager...change this if your gain algorithms class changes names!
#include "AudioEffectCompWDRC2_F32.h"    //change this if you change the name of the algorithm's source code filename
typedef AudioEffectCompWDRC2_F32 GainAlgorithm_t; //change this if you change the algorithm's class name

//now, define the Serial Manager class
class SerialManager {
  public:
    SerialManager(Stream *_s, int n, GainAlgorithm_t *gain_algs, 
          AudioControlTestAmpSweep_F32 &_ampSweepTester,
          AudioControlTestFreqSweep_F32 &_freqSweepTester,
          AudioControlTestFreqSweep_F32 &_freqSweepTester_filterbank,
          AudioEffectFeedbackCancel_F32 &_feedbackCancel)
      : gain_algorithms(gain_algs), 
        ampSweepTester(_ampSweepTester), 
        freqSweepTester(_freqSweepTester),
        freqSweepTester_filterbank(_freqSweepTester_filterbank),  
        feedbackCanceler(_feedbackCancel)
        {
          s = _s;
          N_CHAN = n;
        };
      
    void respondToByte(char c);
    void printHelp(void);
    void incrementChannelGain(int chan, float change_dB);
    void decreaseChannelGain(int chan);

    float channelGainIncrement_dB = 2.5f;  
    int N_CHAN;
  private:
    Stream *s;
    GainAlgorithm_t *gain_algorithms;  //point to first element in array of expanders
    AudioControlTestAmpSweep_F32 &ampSweepTester;
    AudioControlTestFreqSweep_F32 &freqSweepTester;
    AudioControlTestFreqSweep_F32 &freqSweepTester_filterbank;
    AudioEffectFeedbackCancel_F32 &feedbackCanceler;
};

void SerialManager::printHelp(void) {
  s->println();
  s->println("SerialManager Help: Available Commands:");
  s->println("   h: Print this help");
  s->println("   g: Print the gain settings of the device.");
  s->println("   C: Toggle printing of CPU and Memory usage");
  s->println("   l: Toggle printing of pre-gain per-channel signal levels (dBFS)");
  s->println("   L: Toggle printing of pre-gain per-channel signal levels (dBSPL, per DSL 'maxdB')");
  s->println("   A: Self-Generated Test: Amplitude sweep.  End-to-End Measurement.");
  s->println("   F: Self-Generated Test: Frequency sweep.  End-to-End Measurement.");
  s->println("   f: Self-Generated Test: Frequency sweep.  Measure filterbank.");
  s->print("   k: Increase the gain of all channels (ie, knob gain) by "); s->print(channelGainIncrement_dB); s->println(" dB");
  s->print("   K: Decrease the gain of all channels (ie, knob gain) by "); s->print(channelGainIncrement_dB); s->println(" dB");
  s->print("   1,2,3,4,5,6,7,8: Increase linear gain of given channel (1-8) by "); s->print(channelGainIncrement_dB); s->println(" dB");
  s->print("   !,@,#,$,%,^,&,*: Decrease linear gain of given channel (1-8) by "); s->print(channelGainIncrement_dB); s->println(" dB");
  s->println("   D: Toggle between DSL configurations: NORMAL vs FULL-ON");
  s->println("   p: Enable Adaptive Feedback Cancelation.");
  s->println("   P: Disable Adaptive Feedback Cancelation.");
  s->println();
}

//functions in the main sketch that I want to call from here
extern void incrementKnobGain(float);
extern void printGainSettings(void);
extern void printGainSettings(Stream *);
extern void togglePrintMemoryAndCPU(void);
extern void togglePrintAveSignalLevels(bool);
extern void incrementDSLConfiguration(Stream *);

//switch yard to determine the desired action
void SerialManager::respondToByte(char c) {
  switch (c) {
    case 'h': case '?':
      printHelp(); break;
    case 'g': case 'G':
      printGainSettings(); break;
    case 'k':
      incrementKnobGain(channelGainIncrement_dB); break;
    case 'K':   //which is "shift k"
      incrementKnobGain(-channelGainIncrement_dB);  break;
    case '1':
      incrementChannelGain(1-1, channelGainIncrement_dB); break;
    case '2':
      incrementChannelGain(2-1, channelGainIncrement_dB); break;
    case '3':
      incrementChannelGain(3-1, channelGainIncrement_dB); break;
    case '4':
      incrementChannelGain(4-1, channelGainIncrement_dB); break;
    case '5':
      incrementChannelGain(5-1, channelGainIncrement_dB); break;
    case '6':
      incrementChannelGain(6-1, channelGainIncrement_dB); break;
    case '7':
      incrementChannelGain(7-1, channelGainIncrement_dB); break;
    case '8':      
      incrementChannelGain(8-1, channelGainIncrement_dB); break;    
    case '!':  //which is "shift 1"
      incrementChannelGain(1-1, -channelGainIncrement_dB); break;
    case '@':  //which is "shift 2"
      incrementChannelGain(2-1, -channelGainIncrement_dB); break;
    case '#':  //which is "shift 3"
      incrementChannelGain(3-1, -channelGainIncrement_dB); break;
    case '$':  //which is "shift 4"
      incrementChannelGain(4-1, -channelGainIncrement_dB); break;
    case '%':  //which is "shift 5"
      incrementChannelGain(5-1, -channelGainIncrement_dB); break;
    case '^':  //which is "shift 6"
      incrementChannelGain(6-1, -channelGainIncrement_dB); break;
    case '&':  //which is "shift 7"
      incrementChannelGain(7-1, -channelGainIncrement_dB); break;
    case '*':  //which is "shift 8"
      incrementChannelGain(8-1, -channelGainIncrement_dB); break;          
    case 'A':
      //amplitude sweep test
      { //limit the scope of any variables that I create here
        ampSweepTester.setSignalFrequency_Hz(1000.f);
        float start_amp_dB = -100.0f, end_amp_dB = 0.0f, step_amp_dB = 5.0f;
        ampSweepTester.setStepPattern(start_amp_dB, end_amp_dB, step_amp_dB);
        ampSweepTester.setTargetDurPerStep_sec(1.0);
      }
      s->println("Command Received: starting test using amplitude sweep...");
      ampSweepTester.begin();
      while (!ampSweepTester.available()) {delay(100);};
      s->println("Press 'h' for help...");
      break;
    case 'C': case 'c':
      s->println("Command Received: toggle printing of memory and CPU usage.");
      togglePrintMemoryAndCPU(); break;
    case 'D':
      s->println("Command Received: changing DSL configuration...you will lose any custom gain values...");
      incrementDSLConfiguration(s);
      break;
    case 'F':
      //frequency sweep test...end-to-end
      { //limit the scope of any variables that I create here
        freqSweepTester.setSignalAmplitude_dBFS(-70.f);
        float start_freq_Hz = 125.0f, end_freq_Hz = 12000.f, step_octave = powf(2.0,1.0/6.0); //pow(2.0,1.0/3.0) is 3 steps per octave
        freqSweepTester.setStepPattern(start_freq_Hz, end_freq_Hz, step_octave);
        freqSweepTester.setTargetDurPerStep_sec(1.0);
      }
      s->println("Command Received: starting test using frequency sweep, end-to-end assessment...");
      freqSweepTester.begin();
      while (!freqSweepTester.available()) {delay(100);};
      s->println("Press 'h' for help...");
      break; 
    case 'f':
      //frequency sweep test
      { //limit the scope of any variables that I create here
        freqSweepTester_filterbank.setSignalAmplitude_dBFS(-30.f);
        float start_freq_Hz = 125.0f, end_freq_Hz = 12000.f, step_octave = powf(2.0,1.0/6.0); //pow(2.0,1.0/3.0) is 3 steps per octave
        freqSweepTester_filterbank.setStepPattern(start_freq_Hz, end_freq_Hz, step_octave);
        freqSweepTester_filterbank.setTargetDurPerStep_sec(0.5);
      }
      s->println("Command Received: starting test using frequency sweep.  Filterbank assessment...");
      freqSweepTester_filterbank.begin();
      while (!freqSweepTester_filterbank.available()) {delay(100);};
      s->println("Press 'h' for help...");
      break;      
    case 'l':
      s->println("Command Received: toggle printing of per-band ave signal levels.");
      { bool as_dBSPL = false; togglePrintAveSignalLevels(as_dBSPL); }
      break;
    case 'L':
      s->println("Command Received: toggle printing of per-band ave signal levels.");
      { bool as_dBSPL = true; togglePrintAveSignalLevels(as_dBSPL); }
      break;
    case 'p':
      s->println("Command Received: enabling adaptive feedback cancelation.");
      feedbackCanceler.setEnable(true);
      break;
    case 'P':
      s->println("Command Received: disabling adaptive feedback cancelation.");      
      feedbackCanceler.setEnable(false);
      break;
  }
}

void SerialManager::incrementChannelGain(int chan, float change_dB) {
  if (chan < N_CHAN) {
    gain_algorithms[chan].incrementGain_dB(change_dB);
    //s->print("Incrementing gain on channel ");s->print(chan);
    //s->print(" by "); s->print(change_dB); s->println(" dB");
    printGainSettings();  //in main sketch file
  }
}

#endif
