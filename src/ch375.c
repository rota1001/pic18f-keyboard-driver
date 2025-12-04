#include <ch375.h>

struct usb_device_descriptor device_descriptor;
struct usb_config_descriptor config_descriptor;
struct usb_interface_descriptor interface_descriptor;
struct usb_hid_descriptor hid_descriptor;
struct usb_endpoint_descriptor endpoint_descriptor;

inline void CH375_WRITE(uint8_t data)
{
    TRISD = 0;
    LATD = data;
    CH375_WR = 0;
    asm("NOP");
    CH375_WR = 1;
}

inline uint8_t CH375_READ()
{
    CH375_A0 = 0;
    TRISD = 0xFF;
    CH375_RD = 0;
    asm("NOP");
    uint8_t x = PORTD;
    CH375_RD = 1;
    return WREG;
}

static void wait_for_interrupt()
{
    while (PORTBbits.RB0)
        ;
    return;
}

char decode_hid_key(uint8_t modifier, uint8_t keycode)
{
    uint8_t shift = !!(modifier & 0x22);
    if (keycode >= 0x04 && keycode <= 0x1D)
        return (shift ? 'A' : 'a') + (keycode - 0x04);

    if (keycode >= 0x1E && keycode <= 0x27)
        return (char *[2]){"1234567890", "!@#$%^&*()"}[shift][keycode - 0x1E];

    if (keycode > 0x38 || keycode < 0x28)
        return 0;

    return (char *[2]){
        "\n\x00\x00\x00 -=[]\\\x00;'`,./",
        "\n\x00\x00\x00 _+{}|\x00:\"~<>?"}[shift][keycode - 0x28];
}

void usb_handler()
{
    printf("[+] USB Event\n");
    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_CONNECT) {
        printf("[!] Connection Fail\n");
        return;
    }

    printf("[+] Send Reset Signal\n");
    set_usb_mode(USB_MODE_RESET);
    set_usb_mode(USB_MODE_SOF);
    wait_for_interrupt();
    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_CONNECT) {
        printf("[!] Connection Fail\n");
        return;
    }
    printf("[+] USB Device Connected\n");

    CH375_CMD(SET_USB_SPEED);
    CH375_WRITE_DATA(USB_SPEED_LOW_SPEED);

    CH375_CMD(GET_DESCR);
    CH375_WRITE_DATA(0x01);
    wait_for_interrupt();

    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_SUCCESS) {
        printf("[!] Get Device Descriptor Fail\n");
        return;
    }
    printf("[+] Got Device Descriptor\n");

    CH375_CMD(RD_USB_DATA);
    uint8_t len = CH375_READ();
    if (len == 18) {
        uint8_t *ptr = (uint8_t *) &device_descriptor;
        for (uint8_t i = 0; i < len; i++)
            *ptr++ = CH375_READ();
    }
#ifdef DEBUG
    print_device_descriptor(&device_descriptor);
#endif

    CH375_CMD(SET_ADDRESS);
    CH375_WRITE_DATA(0x02);
    wait_for_interrupt();
    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_SUCCESS) {
        printf("[!] Set Address Fail\n");
        return;
    }
    printf("[+] Set Address Success\n");

    CH375_CMD(SET_USB_ADDR);
    CH375_WRITE_DATA(0x02);


    CH375_CMD(GET_DESCR);
    CH375_WRITE_DATA(0x02);
    wait_for_interrupt();

    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_SUCCESS) {
        printf("[!] Get Config Descriptor Fail\n");
        return;
    }
    printf("[+] Got Config Descriptor\n");

    CH375_CMD(RD_USB_DATA);
    len = CH375_READ();
    if (len < 9)
        return;
    uint8_t *ptr = (uint8_t *) &config_descriptor;
    for (uint8_t i = 0; i < 9; i++)
        *ptr++ = CH375_READ();
    len -= 9;
#ifdef DEBUG
    print_config_descriptor(&config_descriptor);
#endif

    if (len < 9)
        return;
    ptr = (uint8_t *) &interface_descriptor;
    for (uint8_t i = 0; i < 9; i++)
        *ptr++ = CH375_READ();
    len -= 9;
#ifdef DEBUG
    print_interface_descriptor(&interface_descriptor);
#endif

    if (len < 9)
        return;
    ptr = (uint8_t *) &hid_descriptor;
    for (uint8_t i = 0; i < 9; i++)
        *ptr++ = CH375_READ();
    len -= 9;
#ifdef DEBUG
    print_interface_descriptor(&interface_descriptor);
#endif

    if (len < 7)
        return;
    ptr = (uint8_t *) &endpoint_descriptor;
    for (uint8_t i = 0; i < 7; i++)
        *ptr++ = CH375_READ();
    len -= 7;
#ifdef DEBUG
    print_endpoint_descriptor(&endpoint_descriptor);
#endif
    CH375_CMD(SET_CONFIG);
    CH375_WRITE_DATA(config_descriptor.bConfigurationValue);

    wait_for_interrupt();
    CH375_CMD(GET_STATUS);
    if (CH375_READ() != USB_INT_SUCCESS) {
        printf("[!] Set Configuration Value fail\n");
        return;
    }

    CH375_CMD(SET_RETRY);
    CH375_WRITE_DATA(0x25);
    CH375_WRITE_DATA(0x85);

    uint8_t yee = 0;
    uint8_t buf[8];
    printf("[+] USB Connected\n");
    while (1) {
        CH375_CMD(SET_ENDP6);
        if (yee == 0)
            CH375_WRITE_DATA(0x80);
        else
            CH375_WRITE_DATA(0xC0);
        yee ^= 1;

        uint8_t addr = endpoint_descriptor.bEndpointAddress & 0xF;
        CH375_CMD(ISSUE_TOKEN);
        CH375_WRITE_DATA((uint8_t) (addr << 4) | DEF_USB_PID_IN);
        wait_for_interrupt();
        CH375_CMD(GET_STATUS);

        if (CH375_READ() != USB_INT_SUCCESS) {
            CH375_CMD(GET_STATUS);
            if (CH375_READ() == USB_INT_DISCONNECT)
                printf("[+] USB Disconnected\n");
            return;
        }


        CH375_CMD(RD_USB_DATA);
        len = CH375_READ();
        if (len <= 8) {
            uint8_t *ptr = buf;
            for (int i = 0; i < len; i++)
                *ptr++ = CH375_READ();

            /** Ignore zero key code and only use the first key code */
            if (buf[2] == 0)
                continue;
            printf("%c", decode_hid_key(buf[0], buf[2]));
        }
    }

    while (1)
        ;
}

void ch375_gpio_init(void)
{
    TRISA &= 0b11111000;
    TRISD = 0xFF;
    CH375_WR = 1;
    CH375_RD = 1;
    TRISB0 = 1;
}


void ch375_init()
{
    ch375_gpio_init();
    __delay_ms(40);
    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.GIE = 1;
    CH375_CMD(GET_IC_VAR);
    printf("IC version: 0x%x\n", CH375_READ());
    set_usb_mode(USB_MODE_SOF);
}