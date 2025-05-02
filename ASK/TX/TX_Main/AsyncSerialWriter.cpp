#include "AsyncSerialWriter.h"

static char TX_BUFFER[TX_BUFFER_SIZE];
static bool BITARRAY[BIT_ARRAY_LENGTH];

// helper c functions
static bool bDataReady;
static uint8_t pos;
static uint16_t rd_ptr;
static uint16_t wr_ptr;
static bool b_carry;

bool generate_even_parity(uint8_t c) {
  int count = 0;
  while (c) {
    count += c & 1; c >>= 1;
  }
  return count % 2 == 0 ? 0 : 1;
}

static bool send_bit() {
  // check if there is data to send in the buffer
  if (!bDataReady) { 
    if (rd_ptr == wr_ptr) return LOW; // no data to send
    else if ((rd_ptr < wr_ptr) || b_carry) { // there's data to send
      char c = TX_BUFFER[rd_ptr++];
      if (rd_ptr == TX_BUFFER_SIZE) rd_ptr = 0;
      if (rd_ptr == 0 && b_carry) b_carry = false;
      // encode_char_to_bitarray
      int i = 0;
      for (; i < DATA_BIT_LENGTH; ++i) {
        BITARRAY[i+1] = bool((c >> i) & 0x01); // LSB-MSB
      }
      BITARRAY[i+1] = generate_even_parity(c);
      pos = 0;
      bDataReady = true;
    }
  }

  if (bDataReady) {
    if (pos < BIT_ARRAY_LENGTH) {
      return BITARRAY[pos++]; // send bit
    } else { 
      pos = 0;
      bDataReady = false;
      return LOW;
    }
  }
  return LOW;
}


// class defenition
AsyncSerialWriter::AsyncSerialWriter() {
  bDataReady = false;
  pos = 0;
  rd_ptr = wr_ptr = 0;
  b_carry = false;
  memset(TX_BUFFER, 0, sizeof(char)*TX_BUFFER_SIZE);
  memset(BITARRAY, 0, sizeof(bool)*BIT_ARRAY_LENGTH);
  BITARRAY[0] = START_BIT;
  BITARRAY[BIT_ARRAY_LENGTH-1] = STOP_BIT;
}

AsyncSerialWriter::~AsyncSerialWriter() { }

bool AsyncSerialWriter::begin(uint32_t baudrate) {
  if (baudrate == 0) {
    fprintf(stderr, "Warning: set baudrate 0\n");
    return false;
  }
  uint32_t duration_us = 1000000 / baudrate;  
  mod_init();
  mod_set_interval(duration_us);
  mod_set_bitwriter(send_bit);
  mod_start();
  return true;
}

void AsyncSerialWriter::write(char c) {
  if (wr_ptr < TX_BUFFER_SIZE) {
    TX_BUFFER[wr_ptr++] = c;   
  } 
  if (wr_ptr == TX_BUFFER_SIZE) {
    wr_ptr = 0;
    b_carry = true;
  } 
}


