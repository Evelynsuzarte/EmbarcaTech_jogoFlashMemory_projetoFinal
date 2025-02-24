#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "matriz_leds.h"
#include "inc/ssd1306.h"
#include "inc/font.h"


#define LED_RED 12
#define LED_GREEN 11
#define LED_BLUE 13
#define BUTTON_A 5
#define BUTTON_B 6
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1
#define I2C_ADDRESS 0x3C
#define PWM_MAX 255
#define PWM_FREQ 20000
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_SDA 14
#define DISPLAY_SCL 15
#define TEMPO_DEBOUNCE 200
#define VALOR_CENTRAL 2048


char texto_LED[20];
ssd1306_t display;
volatile int pontuacao = 0;
volatile bool botaoA_pressionado = false;
volatile bool botaoB_pressionado = false;


void setup() {

    stdio_init_all();

    //LEDS
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    //DISPLAY
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(DISPLAY_SDA, GPIO_FUNC_I2C);                    
    gpio_set_function(DISPLAY_SCL, GPIO_FUNC_I2C);                    
    gpio_pull_up(DISPLAY_SDA);                                        
    gpio_pull_up(DISPLAY_SCL);                                      
    ssd1306_init(&display, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, I2C_ADDRESS, I2C_PORT); 
    ssd1306_config(&display);                                              
    ssd1306_send_data(&display);  
    ssd1306_rect(&display, 31, 63, 8, 8, pintar, !pintar);
    ssd1306_send_data(&display);

    //botao
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

}

void callback_botao(uint gpio, uint32_t events) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_tempo >= TEMPO_DEBOUNCE) {
        if(gpio == BUTTON_A) {
            botaoA_pressionado = true;
        }        
        else if(gpio == BUTTON_B){
            botaoB_pressionado = true;
        }
    
    ultimo_tempo = tempo_atual;
    }
}


void mensagem_inicializa(ssd1306_t display, char texto_LED[20]){

    ssd1306_fill(&display, 0);
    sprintf(texto_LED, "FLASH MEMORY");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    ssd1306_send_data(&display);
    sprintf(texto_LED, "INICIALIZE A SEQUENCIA");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    ssd1306_send_data(&display);
    sprintf(texto_LED, "ESPERE 10 SEGUNDOS E O JOGO JA VAI COMECAR");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    ssd1306_send_data(&display);
    sprintf(texto_LED, "APERTE A PARA AZUL E B PARA VERDE");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    ssd1306_send_data(&display);
}

int main()
{
    stdio_init_all();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &callback_botao);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &callback_botao);

    mensagem_inicializa(display, texto_LED);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
