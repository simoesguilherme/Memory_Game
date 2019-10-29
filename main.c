#include <msp430G2553.h>
#include <msp430.h>

#define RED_LED         BIT1                        // Red LED          (P1.1)      BIT1
#define GREEN_LED       BIT2                        // Green LED        (P1.2)      BIT2
#define BLUE_LED        BIT4                        // Blue LED         (P1.4)      BIT4
#define YELLOW_LED      BIT5                        // Yellow LED       (P1.5)      BIT5

#define TEN_MS      1
#define QUART_SEC   25
#define ONE_SEC     100
#define BLINK       50
#define PAUSE       50

#define ENABLE_PINS 0xFFFE
#define ACLK        0x0100
#define SMCLK       0x0200
#define CONTINUOUS  0x0020
#define UP          0x0010

void Reset(void);
void Wait(int t);

int GetFirstNumber(void);
int GetSecondNumber(void);
void MakeSequence(int sequence[16], int first_number, int second_number);

void BlinkLeds(int sequence[16], int round);

void main(void)
{
    WDTCTL  = WDTPW | WDTHOLD;

    TA0CTL |= ACLK | UP;
    TA0CCR0 = 400;

    TA0CTL |= ACLK | CONTINUOUS;
    TA1CTL |= SMCLK | CONTINUOUS;

    P1DIR |= BLUE_LED | YELLOW_LED | RED_LED | GREEN_LED;
    P1OUT &= (~RED_LED);
    P1OUT &= (~BLUE_LED);
    P1OUT &= (~GREEN_LED);
    P1OUT &= (~YELLOW_LED);

    while(1)
    {
        int first_number = 0;
        first_number = GetFirstNumber();
        Wait(QUART_SEC);
        int second_number = 0;
        second_number = GetSecondNumber();
        int sequence[8] = {0.0};
        MakeSequence(sequence, first_number, second_number);

        int game_state = 1;
        int round = 0;
        while(game_state == 1)
        {
            Wait(ONE_SEC);
            BlinkLeds(sequence, round);

            Wait(TEN_MS);
            if (game_state == 1)
            {
                if (round == 9)
                {
                    round = 0;
                }
                else {round++;}
            }
            Wait(TEN_MS);
        }
    }
}

void Wait(int t)
{
    int i = 0;
    // While the count set has not been reached
    while (i <= t)
    {   // When another 10 milliseconds have expired
        if (TA0CTL & TAIFG)
        {
            // Increase the count and start another 10-millisecond round
            i++;
            TA0CTL &= (~TAIFG);
        }
    }
}

int GetFirstNumber(void)
{
    int first_num = 0;
    first_num = TA0R;
    return first_num;
}

int GetSecondNumber(void)
{
    int second_num = 0;
    second_num = TA1R;
    return second_num;
}


void MakeSequence(int sequence[8], int first_number, int second_number)
{
    int i;
    int first_array[16] = {0.0};
    int second_array[16] = {0.0};
    for (i = 0; i < 16; i++)
    {
        first_array[(15 - i)] = ((first_number >> i) & 0x01);
        second_array[(15 - i)] = ((second_number >> i) & 0x01);
    }
    for (i = 0; i < 3; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 4);
    }
    for (i = 3; i < 6; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 1);
    }
    for (i = 6; i < 8; i++)
    {
        sequence[i] = (first_array[i]) + (second_array[i] * 3);
    }
}

void BlinkLeds(int sequence[16], int round)
{
    int i = 0;
    do
    {
        switch (sequence[i])
        {
        case (0):   P1OUT |= RED_LED;
                    Wait(BLINK);
                    P1OUT &= (~RED_LED);
                    Wait(PAUSE);
                    break;

        case (1):   P1OUT |= GREEN_LED;
                    Wait(BLINK);
                    P1OUT &= (~GREEN_LED);
                    Wait(PAUSE);
                    break;

        case (3):   P1OUT |= BLUE_LED;
                    Wait(BLINK);
                    P1OUT &= (~BLUE_LED);
                    Wait(PAUSE);
                    break;

        case (4):   P1OUT |= YELLOW_LED;
                    Wait(BLINK);
                    P1OUT &= (~YELLOW_LED);
                    Wait(PAUSE);
                    break;
        }

        i = i + 1;

    }
    while (i <= round);
}
