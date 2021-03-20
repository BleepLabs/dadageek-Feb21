int32_t prev[12];

void save_sample_length(uint16_t slot, uint32_t to_save) {
  int16_t startee = slot * 4;
  EEPROM.write(startee, to_save >> 24);
  EEPROM.write(startee + 1, to_save >> 16);
  EEPROM.write(startee + 2, to_save >> 8);
  EEPROM.write(startee + 3, to_save & 0xFF);
}

uint32_t recall_sample_length(uint16_t slot) {
  int16_t startee = slot * 4;
  byte b4 = EEPROM.read(startee);
  byte b3 = EEPROM.read(startee + 1);
  byte b2 = EEPROM.read(startee + 2);
  byte b1 = EEPROM.read(startee + 3);
  uint32_t lt = (b4 << 24) | (b3 << 16) | (b2 << 8) | (b1 );
  return lt;
}

void load_sample_locations() {
    Serial.print("bank# :  bankstart[]  samplelen[] ");
    Serial.println();
  for (byte i = 0; i < number_of_banks; i++) {
    bankstart[i] = (i * sfblocks * 0x10000 * 4);
    samplelen[i] = recall_sample_length(i);
    if (samplelen[i]>=4294967294){
      samplelen[i]=0;
    }
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(bankstart[i]);
    Serial.print(" ");
    Serial.println(samplelen[i]);
  }
}


void startRecording(byte sel) {
  rec_target = sel;
  Serial.print("startRecording address:");
  int16_t rec_count = 0;;
  address = (bankstart[rec_target] / 2) + foffset;
  Serial.println(address);
  queue_left.begin();
  queue_right.begin();


}

void stopRecording() {
  samplelen[rec_target] = ((address) - ((bankstart[rec_target] / 2) + foffset)) / 2;
  save_sample_length(rec_target, samplelen[rec_target]);

  Serial.print("stopRecording  ");
  Serial.print(address);
  Serial.print(" "); Serial.println(samplelen[rec_target]);
  queue_right.end();
  queue_left.end();

  while (queue_left.available() > 0) {
    queue_left.freeBuffer();
    queue_left.clear();
    queue_right.freeBuffer();
    queue_right.clear();
  }

}
void continueRecording() {
  byte q1a = queue_left.available();
  byte q2a = queue_right.available();


  if (q1a == 1 && q2a == 1) {

    int16_t buffer_in_left[(getsize * 4) + 10];
    int16_t buffer_in_right[(getsize * 4) + 10];
    int16_t buffer_out[(getsize * 2) + 10];

    memcpy(buffer_in_left, queue_left.readBuffer(), getsize);
    memcpy(buffer_in_right, queue_right.readBuffer(), getsize);

    for (uint32_t i = 0; i < getsize; i ++) {
      uint16_t j = i * 2;
      buffer_out[j] = buffer_in_left[i];
      buffer_out[j + 1] = buffer_in_right[i];
    }

    AudioStartUsingSPI();
    SerialFlash.write(address * 2, buffer_out, getsize * 2);
    AudioStopUsingSPI();

    queue_left.freeBuffer();
    queue_right.freeBuffer();

    address += getsize;

    if (address > ((rec_size * 2) + (bankstart[rec_target]) / 2)) {
      Serial.println("max bank size reached ");
      stopRecording() ;
    }

  }

}

void eraseBlocks(int estart, int elen) {
  Serial.print("erasing block: ");
  Serial.print(estart);
  Serial.print(" ");
  for (int i = estart; i < estart + elen; i++) {
    uint32_t eb = (i * 0x10000);
    SerialFlash.eraseBlock(eb);
    while (SerialFlash.ready() == false) {
      if (millis() - prev[4] > 1000) {
        prev[4] = millis();
        Serial.print(".");
      }
    }
  }
  Serial.println(" done");

}

void erase_bank(byte bankneum) {
  eraseBlocks(sfblocks * bankneum * 4, sfblocks * 4);
}
