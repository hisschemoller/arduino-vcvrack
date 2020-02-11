#include "Arduino.h"
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <uClock.h>

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define NUM_LEDS 8
#define MIDI_CHANNEL 0
#define PATTERN_LENGTH 16

bool isPlaying = false;
bool isMessage = false;
int pattern[PATTERN_LENGTH] = {100,0,0,0,100,0,0,0,100,0,0,0,100,0,50,0};

void clockOut16PPQN(uint32_t* tick) {
  isMessage = false;
  if (isPlaying) {
    isPlaying = false;
    midiEventPacket_t midiEvent = {0x08, NOTE_OFF | MIDI_CHANNEL, 60, 0};
    MidiUSB.sendMIDI(midiEvent);
    digitalWrite(2, false);
    isMessage = true;
  }
  uint32_t index = *tick % PATTERN_LENGTH;
  if (pattern[index] > 0) {
    isPlaying = true;
    midiEventPacket_t midiEvent = {0x09, NOTE_ON | MIDI_CHANNEL, 60, pattern[index]};
    MidiUSB.sendMIDI(midiEvent);
    digitalWrite(2, true);
    isMessage = true;
  }
  if (isMessage) {
    MidiUSB.flush();
  }
}

void sendMIDIMessage(uint8_t header, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
  midiEventPacket_t midiEvent = {header, byte0, byte1, byte2};
  MidiUSB.sendMIDI(midiEvent);
  MidiUSB.flush();
}

void setup() {
  uClock.init();
  uClock.setClock16PPQNOutput(clockOut16PPQN);
  uClock.setTempo(100);
  uClock.start();
}

void loop() {}
