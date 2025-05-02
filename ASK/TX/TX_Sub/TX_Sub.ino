#if !(SUBCORE == 1)
#error "Core selection is wrong"
#endif

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>   
#include <nuttx/timers/pwm.h>
#include <MP.h>

// pwm settings
static int fd;
static char pwm_devpath[] = "/dev/pwm0";
static struct pwm_info_s info;
static const uint32_t frequency = 40000;
static const uint16_t duty = 0x7fff; 

void setup() {
  MP.begin();
  fd = open(pwm_devpath, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Faital Error: failed to open pwm0 device\n");
    return;
  }

  info.frequency = frequency;
  info.duty      = duty;
  ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));
  ioctl(fd, PWMIOC_START, 0);
}

void loop() {
  int8_t recvid;
  uint32_t data;
  MP.Recv(&recvid, &data);
  if (data) ioctl(fd, PWMIOC_START, 0); 
  else      ioctl(fd, PWMIOC_STOP,  0);
  digitalWrite(LED2, data);
}