
/*
   Chip Audette, OpenAudio, Mar 2018

   MIT License.  Use at your own risk.
*/

#ifndef _SDAudioWriter_SdFAT
#define _SDAudioWriter_SdFAT

//include "kinetis.h"
//include "core_pins.h"

//include <Tympan_Library.h>  //for data types float32_t and int16_t and whatnot
#include <AudioStream.h>   //for AUDIO_BLOCK_SAMPLES
#include "SdFat.h"        //https://github.com/greiman/SdFat

// Preallocate 40MB file.
const uint64_t PRE_ALLOCATE_SIZE = 40ULL << 20;

#define MAXFILE 100

//char *header = 0;
#define MAX_AUDIO_BUFF_LEN (2*AUDIO_BLOCK_SAMPLES)  //Assume stereo (for the 2x).  AUDIO_BLOCK_SAMPLES is from Audio_Stream.h, which should be max of Audio_Stream_F32 as well.

class SDAudioWriter_SdFat
{
  public:
    SDAudioWriter_SdFat() {};
    ~SDAudioWriter_SdFat() {
      if (isFileOpen()) {
        close();
      }
    }
    void setup(void) {
      init();
    };
    void init() {
      if (!sd.begin()) sd.errorHalt("sd.begin failed");
    }
    //write two F32 channels as int16
    int writeF32AsInt16(float32_t *chan1, float32_t *chan2, int nsamps) {
      const int buffer_len = 2 * nsamps; //it'll be stereo, so 2*nsamps
      int count = 0;
      if (file) {
        for (int Isamp = 0; Isamp < nsamps; Isamp++) {
          //convert the F32 to Int16 and interleave
          write_buffer[count++] = (int16_t)(chan1[Isamp] * 32767.0);
          write_buffer[count++] = (int16_t)(chan2[Isamp] * 32767.0);
        }

        // write all audio bytes (512 bytes is most efficient)
        if (flagPrintElapsedWriteTime)  usec = 0;
        file.write((byte *)write_buffer, buffer_len * sizeof(write_buffer[0]));
        if (flagPrintElapsedWriteTime) {
          Serial.print("SD, us="); Serial.println(usec);
        }
      }
      return 0;
    }

    bool open(char *fname) {
      if (file.exists(fname)) {  //maybe this isn't necessary when using the O_TRUNC flag below
        // The SD library writes new data to the end of the
        // file, so to start a new recording, the old file
        // must be deleted before new data is written.
        file.open(fname);
        file.remove();
      }
      //if (file.open(fname, O_CREAT | O_TRUNC | O_RDWR)) { //the O_TRUNC causes this to overwrite the existing file
      //  //if (!file.preAllocate(PRE_ALLOCATE_SIZE)) sd.errorHalt("file.preAllocate failed");
      //}
      file.createContiguous(fname,PRE_ALLOCATE_SIZE); 
      return isFileOpen();
    }

    int close(void) {
      //file.truncate();
      file.close();
      return 0;
    }
    bool isFileOpen(void) {
      if (file) {
        return true;
      } else {
        return false;
      }
    }
//    char* makeFilename(char * filename)
//    { static int ifl = 0;
//      ifl++;
//      if (ifl > MAXFILE) return 0;
//      sprintf(filename, "File%04d.raw", ifl);
//      //Serial.println(filename);
//      return filename;
//    }

    void enablePrintElapsedWriteTime(void) {
      flagPrintElapsedWriteTime = true;
    }
    void disablePrintElapseWriteTime(void) {
      flagPrintElapsedWriteTime = false;
    }

  private:
    //SdFatSdio sd; 
    SdFatSdioEX sd; 
    File file;
    int16_t write_buffer[MAX_AUDIO_BUFF_LEN];
    boolean flagPrintElapsedWriteTime = false;
    elapsedMicros usec;

};


#endif
