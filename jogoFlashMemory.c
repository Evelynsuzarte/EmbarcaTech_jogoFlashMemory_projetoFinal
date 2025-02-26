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

bool compara_array(int sequencia, int resposta){
    int sequencia_quant = sizeof(sequencia) / sizeof(sequencia[0]);
    int resposta_quant = sizeof(resposta) / sizeof(resposta[0]);

    if (sequencia_quant == resposta_quant){
        for (int i = 0; i < sequencia_quant; i++) {
            if (sequencia[i] != resposta[i]) {
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

int exibir_sequencia(PIO pio, uint sm, int nivel, int pontuacao){
    int sequencia[nivel+1]; 
    int cor;

    for(i=0;i< nivel+1;i++){
        sequencia[i] = rand() % 2;
    }

    acende_led(1);
    for(i=0;i<nivel+1;i++){
        cor = sequencia[i];
        if (cor == 0){
            triangulo_azul(PIO pio, uint sm);
        }elif(cor == 1){
            triangulo_vermelho(PIO pio, uint sm);
        }
        sleep_ms(2000);
    }

    if nivel == 5{
        sprintf(texto_LED, "FIM DE JOGO");
        ssd1306_draw_string(&display, texto_LED, 30, 10);
        sprintf(texto_LED, "PONTUACAO");
        ssd1306_draw_string(&display, texto_LED, 10, 20);
        sprintf(texto_LED, pontuacao);
        ssd1306_draw_string(&display, texto_LED, 10, 30);
        ssd1306_send_data(&display);
    }
    acende_led(0);
    return sequencia;
}

void mensagem_inicializa(ssd1306_t display, char texto_LED[20]){

    ssd1306_fill(&display, 0);
    sprintf(texto_LED, "FLASH MEMORY");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    sprintf(texto_LED, "MEMORIZE A SEQUENCIA");
    ssd1306_draw_string(&display, texto_LED, 10, 15);
    ssd1306_send_data(&display);
    sleep_ms(3000);
    ssd1306_fill(&display, 0);
    sprintf(texto_LED, "APERTE A PARA AZUL E B PARA VERDE");
    ssd1306_draw_string(&display, texto_LED, 10, 10);
    sprintf(texto_LED, "ESPERE 10 SEGUNDOS E O JOGO JA VAI COMECAR");  
    ssd1306_draw_string(&display, texto_LED, 10, 20);
    ssd1306_send_data(&display);
    sleep_ms(10000);
}

void triangulo_azul(PIO pio, uint sm){
    Matriz_leds_config azul = {
        //  Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
        // R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.0}}, // Linha 2
        {{0.0, 0.0, 0.2}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.2}, {0.0, 0.0, 0.2}}, // Linha 3
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };
    
   imprimir_desenho(azul, pio, sm);
}

void triangulo_vermelho(PIO pio, uint sm){
    Matriz_leds_config vermelho = {
        //  Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
        // R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
        {{0.0, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}}, // Linha 2
        {{0.0, 0.2, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}}, // Linha 3
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };
    
   imprimir_desenho(vermelho, pio, sm);
}

void apaga_matriz(PIO pio, uint sm){
    Matriz_leds_config apaga = {
        //  Coluna 0          Coluna 1          Coluna 2          Coluna 3          Coluna 4
        // R    G    B       R    G    B       R    G    B       R    G    B       R    G    B
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 0
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 1
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 3
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}}  // Linha 4
    };

    imprimir_desenho(apaga, pio, sm);
}

void acende_led(int led){
    switch (led){
    case 1: 
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        break;
    case 2:
        gpio_put(LED_GREEN, 1);
        break;
    case 3:
        gpio_put(LED_RED, 1);
        break;
    case default:
        gpio_put(LED_GREEN, 0);
        gpio_put(LED_RED, 0);
        break;
    }
}

int main()
{
    stdio_init_all();
    PIO pio = pio0;
    uint sm = configurar_matriz(pio);


    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &callback_botao);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &callback_botao);

    mensagem_inicializa(display, texto_LED);

    int nivel = 1;
    int pontuacao;
    bool array_igual;

    
    while (true) {
        int sequencia_resp; 
        int sequencia; 

        sequencia = exibir_sequencia(pio, uint sm, int nivel, int pontuacao);

        apaga_matriz(pio, sm);

        sprintf(texto_LED, "RESPONDA A SEQUENCIA");
        ssd1306_draw_string(&display, texto_LED, 10, 10);
        sprintf(texto_LED, "A- AZUL");
        ssd1306_draw_string(&display, texto_LED, 10, 20);
        sprintf(texto_LED, "B- VERMELHO");
        ssd1306_draw_string(&display, texto_LED, 10, 30);
        ssd1306_send_data(&display);

        int quantidade = sizeof(sequencia) / sizeof(sequencia[0]);

        if (botaoA_pressionado == true){
            if quantidade == 0{
                sequencia_resp[0] == 0;
            }else{
                sequencia_resp[quantidade-1] == 0;
            }
            botaoA_pressionado == false;

        } else if (botaoB_pressionado == true){
            if quantidade == 0{
                sequencia_resp[0] == 1;
            }else{
                sequencia_resp[quantidade-1] == 1;
            }
            botaoB_pressionado == false;
        }

        array_igual = compara_array(sequencia, sequencia_resp);

        if (array_igual){
            acende_led(2);
            pontuacao=pontuacao+1;
            nivel=nivel+1;

            ssd1306_fill(&display, 0);
            sprintf(texto_LED, "VC ACERTOU");
            ssd1306_draw_string(&display, texto_LED, 30, 30);
            ssd1306_send_data(&display);

        }else{
            acende_led(3);
            ssd1306_fill(&display, 0);
            sprintf(texto_LED, "VC ERROU");
            ssd1306_draw_string(&display, texto_LED, 30, 30);
            sprintf(texto_LED, "PONTUACAO");
            ssd1306_draw_string(&display, texto_LED, 30, 10);
            sprintf(texto_LED, pontuacao);
            ssd1306_draw_string(&display, texto_LED, 30, 30);
            ssd1306_send_data(&display);
        }

        sleep_ms(2000);
        acende_led(4);
    }
}
