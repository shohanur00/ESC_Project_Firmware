#include "debug.h"
#include <stdarg.h>
#include <stdio.h>

#define LOG_BUF_SIZE 1024

static char log_buf1[LOG_BUF_SIZE];
static char log_buf2[LOG_BUF_SIZE];

static char *active_buf = log_buf1;
static char *send_buf   = log_buf2;

static uint16_t log_index = 0;


void Debug_Add_Log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int written = vsnprintf(&active_buf[log_index],
                            LOG_BUF_SIZE - log_index,
                            fmt,
                            args);

    va_end(args);

    if (written > 0)
    {
        log_index += written;

        // overflow protection
        if (log_index >= LOG_BUF_SIZE)
            log_index = LOG_BUF_SIZE - 1;
    }
}

void Debug_Send_Log(void)
{
    if (HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY)
        return;

    // swap buffer
    char *temp = send_buf;
    send_buf = active_buf;
    active_buf = temp;

    uint16_t len = log_index;

    // clear new active buffer
    log_index = 0;
    active_buf[0] = '\0';

    HAL_UART_Transmit_DMA(&huart3, (uint8_t*)send_buf, len);
}


void Debug_Clear_All_Log(void)
{
    log_index = 0;

    memset(log_buf1, 0, LOG_BUF_SIZE);
    memset(log_buf2, 0, LOG_BUF_SIZE);

    active_buf = log_buf1;
    send_buf   = log_buf2;
}

