#include "AsyncSerialReader.h"

static char RX_BUFFER[RX_BUFFER_SIZE];
static char RECV_BYTE;

static uint8_t mode;
static uint8_t pos;
static uint16_t rd_ptr;
static uint16_t wr_ptr;
static bool b_carry;

static uint32_t my_duration_us = 0;

int check_even_parity(uint8_t c, bool parity_bit) {
  int count = 0;
  while (c) {
    count += c & 1; c >>= 1;
  }
  return (count + parity_bit) % 2 == 0;
}

static uint8_t recv_bit(uint8_t sbit, uint32_t duration) {
  if (duration > my_duration_us*2) {
    printf("Warning: duration is too long.\n");
    mode = wait_startbit;
  }

  switch (mode) {
  case wait_startbit:
    if (sbit == HIGH) {
      RECV_BYTE = 0;
      pos = 0;
      mode = bit_recording;
    }
    break;
  case bit_recording:
    RECV_BYTE |= (sbit << pos++);
    if (DATA_BIT_LENGTH == pos) {
      mode = parity_check;
    }
    break;
  case parity_check:
    if (check_even_parity(RECV_BYTE, sbit)){
      mode = wait_stopbit;
    }
    else  mode = wait_startbit;
    break;
  case wait_stopbit:
    if (sbit == LOW) {
      RX_BUFFER[wr_ptr++] = RECV_BYTE;
      if (wr_ptr == RX_BUFFER_SIZE) {
        wr_ptr = 0;
        b_carry = true;
      } 
      if (rd_ptr == 0 && b_carry) b_carry = false;
    } else {
      // transmit error
    }
    mode = wait_startbit;
    break;
  }
  return mode;
}

// class definition
AsyncSerialReader::AsyncSerialReader() {
  mode = wait_startbit;
  pos = 0;
  RECV_BYTE = 0x00;
  memset(RX_BUFFER, 0, sizeof(char)*RX_BUFFER_SIZE);
  rd_ptr = wr_ptr = 0;
  b_carry = false;
}

AsyncSerialReader::~AsyncSerialReader() { 
  demod_stop();
}

bool AsyncSerialReader::begin(uint32_t baudrate) {
  if (baudrate == 0) {
    fprintf(stderr, "Warning: set baudrate 0\n");
    return false;
  }
  my_duration_us = 1000000 / baudrate;
  demod_init();
  demod_set_interval(my_duration_us);
  demod_set_bitrec(recv_bit);
  demod_start();
  return true;
}

char AsyncSerialReader::read() {
  char val = 0x00;
  if (rd_ptr < wr_ptr || b_carry) {
    val = RX_BUFFER[rd_ptr++];
  }
  if (rd_ptr == RX_BUFFER_SIZE) {
    rd_ptr = 0;
  }
   return val;
}


bool AsyncSerialReader::available() {
  if (rd_ptr == wr_ptr) return false;
  else if ((rd_ptr < wr_ptr) || b_carry) return true;
  return false;
}
