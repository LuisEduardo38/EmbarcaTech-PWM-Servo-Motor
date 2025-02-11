#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"

#include "hardware/i2c.h"
#include "bibliotecas/ssd1306.h"
#include "bibliotecas/font.h"

const uint8_t led_red_pino = 13;
const uint8_t led_blue_pino = 12;
const uint8_t led_green_pino = 11;

const uint8_t btn_a = 5;

const uint8_t servo_pino = 22;
volatile uint16_t motor_level = 10;
volatile uint16_t red_level = 200;
volatile uint16_t blue_level = 200;
volatile uint16_t green_level = 200;

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

void iniciar_pinos();
void gpio_irq_handler(uint gpio, uint32_t events);

int main(){
    stdio_init_all();
    iniciar_pinos();

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

    char string01[20],string02[20],string03[20];
    bool red_estado, blue_estado, green_estado, servo_estado;
    red_estado = true;
    blue_estado = false;
    green_estado = false;
    servo_estado = true;

    while(true){
        sprintf(string01, "%d", red_level);
        sprintf(string02, "%d", blue_level);
        sprintf(string03, "%d", green_level);
        pwm_set_gpio_level(led_red_pino, red_level);
        pwm_set_gpio_level(led_blue_pino, blue_level);
        pwm_set_gpio_level(led_green_pino, green_level);

        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
        ssd1306_draw_string(&ssd, string01, 15, 10);
        ssd1306_draw_string(&ssd, string02, 15, 30);
        ssd1306_draw_string(&ssd, string03, 15, 50);
        ssd1306_send_data(&ssd);
        sleep_ms(100);

        if(red_estado == true){
            red_level = red_level + 100;
            if(red_level == 4000){
                red_estado = false;
                blue_estado = true;
            }
        }
        else if((red_estado == false) && (red_level != 200)){
            red_level = red_level - 100;
        }

        if(blue_estado == true){
            blue_level = blue_level + 100;
            if(blue_level == 4000){
                blue_estado = false;
                green_estado = true;
            }
        }
        else if((blue_estado == false) && (blue_level != 200)){
            blue_level = blue_level - 100;
        }

        if(green_estado == true){
            green_level = green_level + 100;
            if(green_level == 4000){
                green_estado = false;
                red_estado = true;
            }
        }
        else if((green_estado == false) && (green_level != 200)){
            green_level = green_level - 100;
        }
    }
}

void iniciar_pinos(){
    gpio_set_function(led_red_pino, GPIO_FUNC_PWM);
    gpio_set_function(led_blue_pino, GPIO_FUNC_PWM);
    gpio_set_function(led_green_pino, GPIO_FUNC_PWM);
    uint slice_numero_red, slice_numero_blue, slice_numero_green;
    slice_numero_red = pwm_gpio_to_slice_num(led_red_pino);
    slice_numero_blue = pwm_gpio_to_slice_num(led_blue_pino);
    slice_numero_green = pwm_gpio_to_slice_num(led_green_pino);
    pwm_set_clkdiv(slice_numero_red, 4.0);
    pwm_set_clkdiv(slice_numero_blue, 4.0);
    pwm_set_clkdiv(slice_numero_green, 4.0);
    pwm_set_wrap(slice_numero_red, 4000);
    pwm_set_wrap(slice_numero_blue, 4000);
    pwm_set_wrap(slice_numero_green, 4000);
    pwm_set_gpio_level(led_red_pino, 100);
    pwm_set_gpio_level(led_blue_pino, 100);
    pwm_set_gpio_level(led_green_pino, 100);
    pwm_set_enabled(slice_numero_red, true);
    pwm_set_enabled(slice_numero_blue, true);
    pwm_set_enabled(slice_numero_green, true);

    gpio_init(btn_a);
    gpio_set_dir(btn_a, GPIO_IN);
    gpio_pull_up(btn_a);

    gpio_set_function(servo_pino, GPIO_FUNC_PWM);
    uint slice_numero = pwm_gpio_to_slice_num(servo_pino);
    pwm_set_clkdiv(slice_numero, 4.0);
    pwm_set_wrap(slice_numero, 4000);
    pwm_set_gpio_level(servo_pino, 100);
    pwm_set_enabled(slice_numero, true);
}

void gpio_irq_handler(uint gpio, uint32_t events){
    reset_usb_boot(0, 0);
}