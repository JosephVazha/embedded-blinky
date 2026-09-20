/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define RCC_AHB1ENR (*(volatile uint32_t *) 0x40023830)
#define RCC_APB1ENR (*(volatile uint32_t *) 0x40023840)
#define RCC_APB2ENR (*(volatile uint32_t *) 0x40023844)

#define GPIOA_MODER     (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_OTYPER    (*(volatile uint32_t *)(0x40020000UL + 0x04UL))
#define GPIOA_ODR       (*(volatile uint32_t *)(0x40020000UL + 0x14UL))

#define GPIOC_MODER     (*(volatile uint32_t *)0x40020800UL)

#define SYSCFG_EXTICR4  (*(volatile uint32_t *)(0x40013800UL + 0x14UL))

#define EXTI_IMR        (*(volatile uint32_t *)0x40013C00UL)
#define EXTI_FTSR       (*(volatile uint32_t *)(0x40013C00UL + 0x0CUL))
#define EXTI_PR         (*(volatile uint32_t *)(0x40013C00UL + 0x14UL))

#define NVIC_ISER0      (*(volatile uint32_t *)0xE000E100UL)
#define NVIC_ISER1      (*(volatile uint32_t *)0xE000E104UL)

#define TIM2_CR1        (*(volatile uint32_t *)0x40000000UL)
#define TIM2_DIER       (*(volatile uint32_t *)(0x40000000UL + 0x0CUL))
#define TIM2_SR         (*(volatile uint32_t *)(0x40000000UL + 0x10UL))
#define TIM2_PSC        (*(volatile uint32_t *)(0x40000000UL + 0x28UL))
#define TIM2_ARR        (*(volatile uint32_t *)(0x40000000UL + 0x2CUL))


#define LED_OFF_STATE     0
#define LED_ON_STATE      1
#define LED_FLASH_STATE   2

volatile uint8_t led_state = LED_OFF_STATE;


void LED_ON(void) {
	GPIOA_ODR |= (1u << 5);
}

void LED_OFF(void){
	GPIOA_ODR &= ~(1u << 5);
}

void LED_INIT(void) {

    /* Enable GPIOA clock */
    RCC_AHB1ENR |= (1u << 0);

    //	RCC_APB1ENR |= (1u << 0);


    GPIOA_MODER &= ~(3u << 10);
    GPIOA_MODER |=  (1u << 10);

    /*Push-pull */
    GPIOA_OTYPER &= ~(1u << 5);
    //GPIOA_ODR &= ~(1u << 5);
    LED_OFF();
}

//	RCC_AHB1ENR |= (1u << 0);
//
//	GPIO_MODER &= ~(0x3u << (5 * 2));
//	GPIO_MODER |= (0x2u << (5 * 2));
//
//	GPIO_AFRL &= ~(0xFU << (5 * 4));
//	GPIO_AFRL |= (0x1u << (5 * 2));
//
//
//
//	GPIOA_CCMR1 &= ~(0x3u <<0);
//	GPIOA_CCMR1 &= ~(0x7u <<4);
//	GPIOA_CCMR1 |= (0x6u <<4);
//	GPIOA_CCMR1 |= ~(0x1u <<3);
//
//
//	TIM2_CR1 |= (1u << 0);
//	TIME2_PSC = 16000 - 1;
//	TIME2_ARR = 1000 - 1;
//
//	TIM2_CCER |= (1u <<0);
//}

void BUTTON_INIT(void)
{
    RCC_AHB1ENR |= (1u << 2);

    /* PC13 = input */
    GPIOC_MODER &= ~(3u << 26);

    /* Enable SYSCFG clock */
    RCC_APB2ENR |= (1u << 14);

    /* Connect EXTI13 to PC13 */
    SYSCFG_EXTICR4 &= ~(0xFu << 4);
    SYSCFG_EXTICR4 |=  (0x2u << 4);

    /* Falling-edge trigger */
    EXTI_FTSR |= (1u << 13);

    /* Clear pending flag */
    EXTI_PR = (1u << 13);

    /* Unmask EXTI13 */
    EXTI_IMR |= (1u << 13);

    /* Enable Button interrupt */
    NVIC_ISER1 |= (1u << 8);
}
void TIM2_INIT(void)
{
    RCC_APB1ENR |= (1u << 0);

    TIM2_PSC = 15999;
    TIM2_ARR = 999;

    /* Enable update interrupt */
    TIM2_DIER |= (1u << 0);

    /* Clear pending update flag */
    TIM2_SR &= ~(1u << 0);

    /* Enable TIM2 interrupt in NVIC */
    NVIC_ISER0 |= (1u << 28);

    /* Start timer */
    TIM2_CR1 |= (1u << 0);
}


void EXTI1_Button(void)
{
    if (EXTI_PR & (1u << 13))
    {
        EXTI_PR = (1u << 13);

        if (led_state == LED_OFF_STATE)
        {
            led_state = LED_ON_STATE;
            LED_ON();
        }
        else if (led_state == LED_ON_STATE)
        {
            led_state = LED_FLASH_STATE;
            LED_ON();
        }
        else
        {
            led_state = LED_OFF_STATE;
            LED_OFF();
        }
    }
}
void TIM2_I(void)
{
    if (TIM2_SR & (1u << 0))
    {
        TIM2_SR &= ~(1u << 0);

        if (led_state == LED_FLASH_STATE)
        {
            if (GPIOA_ODR & (1u << 5))
            {
                LED_OFF();
            }
            else
            {
                LED_ON();
            }
        }
    }
}

int main(void)
{
	LED_INIT();
    BUTTON_INIT();
    TIM2_INIT();
    while (1)
    {
    }
}

//void EXTI_Button(void)
//{
//    if (EXTI_PR & (1u << 13))
//    {
//        /* Clear pending flag */
//        EXTI_PR = (1u << 13);
//
//        if (GPIOA_ODR & (1u << 5))
//        {
//            LED_OFF();
//        }
//        else
//        {
//            LED_ON();
//        }
//    }
//}
