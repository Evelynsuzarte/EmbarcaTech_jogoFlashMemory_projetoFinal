#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "matriz_leds.h"
#include "inc/ssd1306.h"
#include "inc/font.h"


#define LED_RED 13
#define LED_GREEN 12
#define LED_BLUE 11
#define BUTTON_A 5
#define BUTTON_B 6
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1
#define I2C_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_SDA 14
#define DISPLAY_SCL 15
#define TEMPO_DEBOUNCE 200
#define VALOR_CENTRAL 2048


volatile char texto_LED[20];
ssd1306_t display;
volatile uint32_t ultimo_tempo = 0;
volatile int pontuacao = 0;
volatile int contador = 0;
volatile bool botaoA_pressionado = false;
volatile bool botaoB_pressionado = false;


void setup() {

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
    ssd1306_fill(&display, 0);
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
            contador++;
        }        
        else if(gpio == BUTTON_B){
            botaoB_pressionado = true;
            contador++;
        }
    
    ultimo_tempo = tempo_atual;
    }
}


void mensagem_inicializa(ssd1306_t display){
    ssd1306_draw_string(&display, "FLASH MEMORY", 10, 10);
    ssd1306_draw_string(&display, "MEMORIZE", 20, 40);
    ssd1306_send_data(&display);
    sleep_ms(2000);
    ssd1306_fill(&display, 0);
    ssd1306_draw_string(&display, "APERTE", 10, 10);
    ssd1306_draw_string(&display, "A - AZUL", 10, 20);
    ssd1306_draw_string(&display, "B - VERDE", 10, 30);
    sleep_ms(3000);
    ssd1306_fill(&display, 0);
    ssd1306_send_data(&display);
    ssd1306_draw_string(&display, "ESPERE 10 SEG", 10, 10);
    ssd1306_draw_string(&display, "E O JOGO JA", 10, 20);
    ssd1306_draw_string(&display, "VAI COMECAR", 10, 30);
    ssd1306_send_data(&display);
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
        {{0.0, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.0, 0.0, 0.0}}, // Linha 2
        {{0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}, {0.2, 0.0, 0.0}}, // Linha 3
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
    gpio_put(LED_RED, 0);
    gpio_put(LED_GREEN, 0);

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
        case 4:
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_RED, 0);
            break;
        }
}


int contar_itens(int sequencia[40]){
    int contador_item = 0, i=0;

    while(sequencia[i] != 0){
        contador_item ++;
        i++;
    }

    return contador_item;
}

bool compara_array(int sequencia[40], int resposta[40]){
    int i;
    int sequencia_quant = contar_itens(sequencia);
    int resposta_quant = contar_itens(resposta);

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

void exibir_sequencia(PIO pio, uint sm, int nivel, int sequencia[40]){
    int cor, i, j;

    printf("entrou no exibir_sequencia\n ");
    printf("nivel %d \n", nivel);

    for(i=0; i<nivel+1; i++){
        sequencia[i] = (rand() % 2) + 1;
        printf("item sequencia - %d\n", sequencia[i]);
    }

    for(i=0;i<contar_itens(sequencia);i++){
        printf("item sequencia final - %d\n", sequencia[i]);
    }

    acende_led(1);
    for(i=0;i<nivel+1;i++){
        cor = sequencia[i];
        if (cor == 1){
            printf("cor azul\n");
            triangulo_azul(pio,sm);
            apaga_matriz(pio,sm);
            sleep_ms(1000);
        }else if(cor == 2){
            printf("cor vermelho\n");
            triangulo_vermelho(pio,sm);
            apaga_matriz(pio,sm);
            sleep_ms(1000);
        }
        sleep_ms(2000);
    }

    apaga_matriz(pio,sm);
}
int main()
{
    stdio_init_all();
    PIO pio = pio0;
    uint sm = configurar_matriz(pio);
    setup();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_RISE, true, &callback_botao);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_RISE, true, &callback_botao);

    apaga_matriz(pio,sm);
    mensagem_inicializa(display);
    sleep_ms(10000);
    ssd1306_fill(&display, 0);
    ssd1306_send_data(&display);

    int nivel = 1;
    bool array_igual;

    int sequencia_resp[40] = {0}; 
    int sequencia[40] = {0};
    int quantidade=0,quantidade_resp=0;


    while (true) {
        
        char pont;
        bool continua = true;

        if(nivel < 5){
            exibir_sequencia(pio, sm, nivel, sequencia);

            ssd1306_draw_string(&display, "APERTE ", 10, 10);
            ssd1306_draw_string(&display, "A- AZUL", 10, 20);
            ssd1306_draw_string(&display, "B- VERMELHO", 10, 30);
            ssd1306_send_data(&display);

            int i;
            

            sleep_ms(10000);

           while(continua == true){
                quantidade = contar_itens(sequencia);
                quantidade_resp = contar_itens(sequencia_resp);

                if (quantidade == nivel+1){
                    if (botaoA_pressionado == true){
                        if (quantidade_resp == 0){
                            sequencia_resp[0] = 1;
                        }else{
                            sequencia_resp[quantidade_resp] = 1;
                        }
                        botaoA_pressionado = false;
                        continua = true;
                        sleep_ms(200);
        
                    } if (botaoB_pressionado == true){
                        if (quantidade_resp == 0){
                            sequencia_resp[0] = 2;
                        }else{
                            sequencia_resp[quantidade_resp] = 2;
                        }
                        botaoB_pressionado = false;
                        continua = true;
                        sleep_ms(200);
                    }
                    else{
                        break;
                    }

                }
                printf("quatidade = %d \n", quantidade);

                

            }


            for (int i = 0; i < contar_itens(sequencia_resp); i++) {
                printf("item resposta - %d\n", sequencia_resp[i]);
                printf("item sequencia - %d\n", sequencia[i]);
            }
            acende_led(4);


            quantidade = contar_itens(sequencia_resp);
            printf("quatidade = %d \n", quantidade);

            array_igual = compara_array(sequencia, sequencia_resp);

            printf("igual = %d \n", array_igual);


            if (array_igual){
                acende_led(2);
                pontuacao=pontuacao+1;
                nivel=nivel+1;

                printf("Acertou\n");
                contador = 0;
                ssd1306_fill(&display, 0);
                ssd1306_draw_string(&display, "VC ACERTOU", 30, 30);
                ssd1306_send_data(&display);

            }else{
                printf("Errou\n");
                pont = pontuacao + '0';  
                acende_led(3);
                ssd1306_fill(&display, 0);
                ssd1306_draw_string(&display, "VC ERROU", 30, 10);
                ssd1306_draw_string(&display, "PONTUACAO", 30, 20);
                ssd1306_draw_char(&display, pont, 30, 60);
                sleep_ms(3000);
                ssd1306_fill(&display, 0);
                ssd1306_send_data(&display);
                
                nivel = 1;
                acende_led(4);
                
                for (int i = 0; i < contar_itens(sequencia_resp); i++) {
                    sequencia_resp[i] = 0;
                }
                mensagem_inicializa(display);
                
            }
        }else{
            pont = pontuacao + '0';  
            ssd1306_draw_string(&display, "FIM DE JOGO", 30, 10);
            ssd1306_draw_string(&display, "PONTUACAO", 10, 20);
            ssd1306_draw_char(&display, pont, 10, 60);
            sleep_ms(2000);
            ssd1306_fill(&display, 0);
            ssd1306_send_data(&display);
        }   
        sleep_ms(200);
    }
}
