#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"

#include "hardware/i2c.h"
#include "bibliotecas/ssd1306.h"
#include "bibliotecas/font.h"

const uint8_t led_red_pino = 11;
const uint8_t led_blue_pino = 12;
const uint8_t led_green_pino = 13;

const uint8_t btn_a = 5;

const uint8_t servo_pino = 22;

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

void iniciar_pinos();
void gpio_irq_handler(uint gpio, uint32_t events);

int main(){
    stdio_init_all();
    iniciar_pinos();

    //pwm_set_clkdiv();

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    bool cor = true;

    gpio_set_irq_enabled_with_callback(btn_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
        ssd1306_draw_string(&ssd, "VERM.: TRUE", 15, 10);
        ssd1306_draw_string(&ssd, "AZUL: FALSE", 15, 30);
        ssd1306_draw_string(&ssd, "VERDE: TRUE", 15, 50);
        ssd1306_send_data(&ssd);
        sleep_ms(1000);
    }
}

void iniciar_pinos(){
    gpio_init(led_red_pino);
    gpio_init(led_blue_pino);
    gpio_init(led_green_pino);
    gpio_set_dir(led_red_pino, GPIO_OUT);
    gpio_set_dir(led_blue_pino, GPIO_OUT);
    gpio_set_dir(led_green_pino, GPIO_OUT);
    gpio_put(led_red_pino, false);
    gpio_put(led_blue_pino, false);
    gpio_put(led_green_pino, false);

    gpio_init(btn_a);
    gpio_set_dir(btn_a, GPIO_IN);
    gpio_pull_up(btn_a);
    
    gpio_init(servo_pino);
    gpio_set_function(servo_pino, GPIO_FUNC_PWM);
}

void gpio_irq_handler(uint gpio, uint32_t events){
    reset_usb_boot(0, 0);
}