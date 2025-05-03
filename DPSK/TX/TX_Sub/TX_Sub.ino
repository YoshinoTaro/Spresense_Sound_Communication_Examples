#if !(SUBCORE == 1)
#error "Core selection is wrong"
#endif

#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>   
#include <nuttx/timers/pwm.h>
#include <MP.h>

/* Use CMSIS library */
#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <arm_math.h>

static const uint32_t target_frequency =  500;
static const uint32_t timer_interval_us =  10;
static const uint32_t samples_per_cycle = 1000000 / (target_frequency * timer_interval_us);
static uint32_t duty[samples_per_cycle]; 
static const uint32_t pwm_frequency =  1000000; // 1MHz

// target frequency = 1000000 usec / (saples_per_cycle * timer_interval_us)


char pwm_devpath[] = "/dev/pwm0";
int fd;
struct pwm_info_s info;
 
static bool b_invert = false;
static int count = 0;
unsigned int changeDuty() {
  info.duty = duty[count++];
  ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));  
  if (count == samples_per_cycle) count = 0;
  if (b_invert && count == samples_per_cycle / 4) {
    count += samples_per_cycle/2;
    if (count >= samples_per_cycle) count -= samples_per_cycle;
    b_invert = false;
  }
  return timer_interval_us;
}


void setup() {

  MP.begin();
  MP.RecvTimeout(MP_RECV_POLLING);

  printf("setup /dev/pwm0\n");
  /* Open the PWM device for reading */
  fd = open(pwm_devpath, O_RDONLY);
  if (fd < 0) {
    printf("error: failed to open pwm0 device\n");
    return;
  }

  /* Configure the characteristics of the pulse train */
  info.frequency = pwm_frequency;
  info.duty      = 0x0000;

  ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&info));

  for (uint32_t n = 0; n < samples_per_cycle; ++n) {
    float angle = arm_sin_f32(2*M_PI*n/samples_per_cycle);
    duty[n] = (uint32_t)((angle + 1.0)*0xFFFF/2);
    //Serial.println(String(samples_per_cycle) + " ," + String(duty[n]));  
  }

  ioctl(fd, PWMIOC_START, 0);
  attachTimerInterrupt(changeDuty, timer_interval_us);
}

void loop() {
  int8_t recvid;
  uint32_t data;
  int ret = MP.Recv(&recvid, &data);
  if (ret < 0) return;
  b_invert = true;

  digitalWrite(LED1, data);
  digitalWrite(LED2, !data);  
}
