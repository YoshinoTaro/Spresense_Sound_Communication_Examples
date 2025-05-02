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
static const uint32_t mark_frequency = 41000;
static const uint32_t space_frequency = 38000;
static const uint16_t duty = 0x7fff; 

void setup() {
  MP.begin();
  fd = open(pwm_devpath, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Faital Error: failed to open pwm0 device\n");
    return;
  }

  info.frequency = space_frequency;
  info.duty      = duty;
  ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));
  ioctl(fd, PWMIOC_START, 0);
}

void loop() {
  int8_t recvid;
  uint32_t data;
  MP.Recv(&recvid, &data);
  if (data) {
    info.frequency = mark_frequency; 
    ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info)); 
  } else {
    info.frequency = space_frequency;
    ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));

  } 
  digitalWrite(LED1, data);
  digitalWrite(LED2, !data);  
}