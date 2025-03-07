#include "xuartps.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#define UART_DEVICE_ID  XPAR_PS7_UART_1_DEVICE_ID
#define INTC_DEVICE_ID  XPAR_SCUGIC_0_DEVICE_ID
#define UART_INT_ID     XPAR_XUARTPS_1_INTR

XUartPs Uart;       // UART instance
XScuGic Intc;       // Interrupt controller instance
u8 RecvBuffer[100]; // Buffer for received data

// ✅ FIXED: Interrupt Handler
void UartRecvHandler(void *CallBackRef, u32 Event, unsigned int EventData) {
    XUartPs *UartInst = (XUartPs *)CallBackRef;

    if (Event == XUARTPS_EVENT_RECV_DATA) {
        int ReceivedCount = XUartPs_Recv(UartInst, RecvBuffer, EventData);
        xil_printf("Interrupt Handler: ");
        for (int i = 0; i < ReceivedCount; i++) {
            xil_printf("%c", RecvBuffer[i]); // Print received characters
        }
        xil_printf("\r\n");

    }
}

// ✅ FIXED: UART Initialization
int SetupUart() {
    XUartPs_Config *Config;
    
    // Lookup configuration
    Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (!Config) {
        xil_printf("UART Config lookup failed\r\n");
        return XST_FAILURE;
    }
    
    // Initialize the UART driver
    int Status = XUartPs_CfgInitialize(&Uart, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("UART Initialization failed\r\n");
        return Status;
    }

    // Set baud rate (115200)
    XUartPs_SetBaudRate(&Uart, 115200);

    // Enable FIFO mode for receiving
    XUartPs_SetFifoThreshold(&Uart, 1);

    // Register callback function
    XUartPs_SetHandler(&Uart, UartRecvHandler, &Uart);

    u32  IntrMask =
    XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
    XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
    XUARTPS_IXR_RXOVR;

    XUartPs_SetInterruptMask(&Uart, IntrMask);

    return XST_SUCCESS;
}

// ✅ FIXED: Interrupt Controller Setup
int SetupInterrupts() {
    XScuGic_Config *IntcConfig;
    
    // Lookup interrupt controller config
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (!IntcConfig) {
        xil_printf("Interrupt Controller Config lookup failed\r\n");
        return XST_FAILURE;
    }

    // Initialize interrupt controller
    int Status = XScuGic_CfgInitialize(&Intc, IntcConfig, IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("Interrupt Controller Initialization failed\r\n");
        return Status;
    }

    // Connect UART interrupt
    Status = XScuGic_Connect(&Intc, UART_INT_ID,
                             (Xil_ExceptionHandler)XUartPs_InterruptHandler, &Uart);
    if (Status != XST_SUCCESS) {
        xil_printf("UART Interrupt Connection failed\r\n");
        return Status;
    }

    // Enable UART interrupt
    XScuGic_Enable(&Intc, UART_INT_ID);

    // ✅ Register Exception Handler for Interrupts
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler, &Intc);
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

// ✅ FIXED: Main Function
int main() {
    xil_printf("Initializing UART...\r\n");

    // Setup UART
    if (SetupUart() != XST_SUCCESS) {
        xil_printf("UART Setup Failed!\r\n");
        return -1;
    }

    // Setup Interrupts
    if (SetupInterrupts() != XST_SUCCESS) {
        xil_printf("Interrupt Setup Failed!\r\n");
        return -1;
    }

    xil_printf("UART Initialized. Waiting for data...\r\n");

    while (1)// Keep running
    {
    	if (XUartPs_IsReceiveData(Uart.Config.BaseAddress)) 
        {
    	        xil_printf("Polling Received: %c\r\n", XUartPs_RecvByte(Uart.Config.BaseAddress));
        }
    }
    return 0;
}
