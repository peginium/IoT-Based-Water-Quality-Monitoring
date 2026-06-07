#include <msp430.h>
#include <stdint.h>
#include "i2c.h"
#include "ssd1306.h"
#include "ds18b20.h"

// ==========================================
// --- UART (HC-05) BAŞLATMA FONKSİYONLARI ---
// ==========================================
void uart_init(void) {
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;
}

void uart_print(char *str) {
    while (*str != '\0') {
        while (!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = *str++;
    }
}

// ==========================================
// --- ADC10  ---
// ==========================================
void adc_init(void) {
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON;   
    ADC10AE0 |= BIT4 + BIT5;
}

uint16_t adc_read(uint8_t channel) {
    ADC10CTL0 &= ~ENC;                           
    ADC10CTL1 = (channel << 12) + SHS_0 + ADC10DIV_0 + ADC10SSEL_0; 
    ADC10CTL0 |= ENC + ADC10SC;                  
    while (ADC10CTL1 & ADC10BUSY);               
    return ADC10MEM;                             
}

// ==========================================
// --- ANA PROGRAM ---
// ==========================================
void main(void) {
    WDTCTL = WDTPW + WDTHOLD;                    

    if (CALBC1_1MHZ == 0xFF) { while(1); }
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    __delay_cycles(100000);                      

    i2c_init();                               
    ssd1306_init();                           
    ssd1306_clearDisplay();                   
    adc_init();
    uart_init();

    ssd1306_printText(0, 0, "--- SU KALITESI ---");
    ssd1306_printText(0, 2, "Temp :");
    ssd1306_printText(0, 3, "TDS  :");
    ssd1306_printText(0, 4, "pH   :");
    ssd1306_printText(0, 6, "Genel WQI :");

    while(1) {
        char temp_str[10] = "0.0"; 
        char ph_str[6] = "0.0";
        
        // --- 1. SICAKLIK OKUMA (DS18B20) ---
        int16_t raw_temp = ds18b20_get_temp();
        float gercek_sicaklik = 25.0; 
        
        if (raw_temp != 9999) {
            int16_t tam_kisim = raw_temp >> 4;
            int16_t ondalik_kisim = ((raw_temp & 0x0F) * 10) / 16; 
            gercek_sicaklik = (float)tam_kisim + ((float)ondalik_kisim / 10.0);

            temp_str[0] = (tam_kisim / 10) + '0'; 
            temp_str[1] = (tam_kisim % 10) + '0'; 
            temp_str[2] = '.';                    
            temp_str[3] = ondalik_kisim + '0';    
            temp_str[4] = '\0';
            
            ssd1306_printText(50, 2, temp_str);
            ssd1306_printText(75, 2, "C");
        } else {
            temp_str[0] = '0'; temp_str[1] = '\0';
            ssd1306_printText(50, 2, "HATA ");
        }

        // --- 2. TDS OKUMA VE HESAPLAMA (P1.4) ---
        uint16_t tds_adc = adc_read(4); 
        float tds_voltaj = ((float)tds_adc / 1023.0) * 3.3;
        float tds_kompanzasyon = 1.0 + 0.02 * (gercek_sicaklik - 25.0);
        float tds_komp_voltaj = tds_voltaj / tds_kompanzasyon;
        
        float tds_degeri = (133.42 * tds_komp_voltaj * tds_komp_voltaj * tds_komp_voltaj 
                          - 255.86 * tds_komp_voltaj * tds_komp_voltaj 
                          + 857.39 * tds_komp_voltaj) * 0.5;
        if (tds_degeri < 0) tds_degeri = 0;

        uint32_t final_tds = (uint32_t)tds_degeri;
        ssd1306_printText(50, 3, "        "); 
        ssd1306_printUI32(50, 3, final_tds, 0);
        ssd1306_printText(80, 3, "ppm");

        // --- 3. pH OKUMA VE HESAPLAMA (P1.5) ---
        uint16_t ph_adc = adc_read(5);  
        float ph_voltaj = ((float)ph_adc / 1023.0) * 3.3;
        float ph_degeri = 7.0 + ((1.5 - ph_voltaj) / 0.18);
        
        if (ph_degeri < 0.0) ph_degeri = 0.0;
        if (ph_degeri > 14.0) ph_degeri = 14.0;

        int16_t ph_tam = (int16_t)ph_degeri;
        int16_t ph_ondalik = (int16_t)((ph_degeri - ph_tam) * 10);
        
        ph_str[0] = (ph_tam / 10) ? ((ph_tam / 10) + '0') : ' '; 
        ph_str[1] = (ph_tam % 10) + '0';
        ph_str[2] = '.';
        ph_str[3] = ph_ondalik + '0';
        ph_str[4] = '\0';
        
        ssd1306_printText(50, 4, "      "); 
        ssd1306_printText(50, 4, ph_str);

        // --- 4. WQI (WATER QUALITY INDEX) HESABI ---
        float ph_fark = ph_degeri - 7.0;
        if (ph_fark < 0) ph_fark = -ph_fark; 
        float q_ph = 100.0 - (ph_fark * 20.0);
        if (q_ph < 0) q_ph = 0;

        float q_tds = 100.0 - (tds_degeri / 5.0); 
        if (q_tds < 0) q_tds = 0;

        float temp_fark = gercek_sicaklik - 22.0;
        if (temp_fark < 0) temp_fark = -temp_fark; 
        float q_temp = 100.0 - (temp_fark * 3.0);
        if (q_temp < 0) q_temp = 0;

        uint8_t wqi_skoru = (uint8_t)((q_ph * 0.45) + (q_tds * 0.45) + (q_temp * 0.10));

        ssd1306_printText(75, 6, "   "); 
        ssd1306_printUI32(75, 6, wqi_skoru, 0);
        ssd1306_printText(100, 6, "/100");

        ssd1306_printText(0, 7, "Durum: ");
        if (wqi_skoru >= 80)      ssd1306_printText(40, 7, "MUKEMMEL ");
        else if (wqi_skoru >= 60) ssd1306_printText(40, 7, "IYI      ");
        else if (wqi_skoru >= 40) ssd1306_printText(40, 7, "ZAYIF    ");
        else                      ssd1306_printText(40, 7, "ICILEMEZ!");

        // --- 5. BLUETOOTH (HC-05) İLE VERİ AKTARIMI (JSON FORMATI) ---
        uart_print("{\"t\":");
        uart_print(temp_str);
        
        uart_print(",\"tds\":");
        char buffer_tds[10];
        ssd1306_ultoa(final_tds, buffer_tds);  
        uart_print(buffer_tds);
        
        uart_print(",\"ph\":");
        uart_print(ph_str);                    
        
        uart_print(",\"wqi\":");
        char buffer_wqi[10];
        ssd1306_ultoa(wqi_skoru, buffer_wqi);
        uart_print(buffer_wqi);
        
        uart_print("}\r\n");

        __delay_cycles(1000000);
    }
}