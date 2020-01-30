
#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdlib.h>
#include <stdio.h>

#include "rtx_os.h" // ARM::CMSIS:RTOS:Keil RTX

#if !defined(__CC_ARM) && defined(__ARMCC_VERSION) && !defined(__OPTIMIZE__)
    /*
    Without this directive, it does not start if -o0 optimization is used and the "main"
    function without parameters.
    see http://www.keil.com/support/man/docs/armclang_mig/armclang_mig_udb1499267612612.htm
    */
    __asm (".global __ARM_use_no_argv\n\t" "__ARM_use_no_argv:\n\t");
#endif

// -- id
osThreadId_t id = NULL;

osMutexId_t mutex_id;

// -- main thread func
void thread_main(void *param)
{
    (void)param;
    
    printf("<thread_main> start\r\n");
    
    for (;;)
    {

    }
};

// -- static thread
void thread_static(void *param)
{
    (void)param;
    
    printf("<thread_static> start\r\n");
    
    osMutexAcquire(mutex_id, osWaitForever);
    
    for (;;)
    {
        osThreadExit();
    }
};

uint64_t stack[0x1000/sizeof(uint64_t)]; 
osThreadAttr_t attr =
{
    .name       = "thread_static",
    .attr_bits  = osThreadJoinable,
//    .cb_mem     = &tcb,
//    .cb_size    = sizeof(osRtxThread_t),
    .stack_mem  = stack,
    .stack_size = sizeof(stack),
    .priority   = osPriorityNormal,
};

// -- mutex
const osMutexAttr_t Thread_Mutex_attr =
{
  "myThreadMutex",                          // human readable mutex name
  osMutexRobust | osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
};

// -- main function
int main(void)
{
    osVersion_t vers;

    osKernelGetInfo(&vers, NULL, 0);
    
    printf("<cpp_os> Operation system info:\r\n"
           "  API version: %d.%d.%d\r\n"
           "  kernel version: %d.%d.%d\r\n"
           "  kernel id: " osRtxKernelId "\r\n",
           vers.api    / 10000000, (vers.api    % 10000000) / 10000, vers.api    % 10000,
           vers.kernel / 10000000, (vers.kernel % 10000000) / 10000, vers.kernel % 10000);
           
    printf("<cpp_os> Compiller version: %d.%d.%d\r\n",
           __ARMCC_VERSION / 1000000,
           (__ARMCC_VERSION % 1000000) / 10000,
           __ARMCC_VERSION % 10000);

    printf("<cpp_os> compilation date and time: " __DATE__ " [" __TIME__ "]\r\n");


    osKernelInitialize(); // initialize RTX

    // Threads create
    osThreadNew(thread_main, NULL, NULL);
    
    id = osThreadNew(thread_static, NULL, &attr);
    
    mutex_id = osMutexNew(&Thread_Mutex_attr);
    

    osKernelStart(); // start RTX kernel
    
    for(;;);
    return 0;
}
