// Youtube >>> H.M.Ç >>> https://www.youtube.com/@h.m.c3847
// Linkedin >>> https://www.linkedin.com/in/hasan-mesut-%C3%A7etin-975744286/

// Touch screen kalibrasyon değerleri - dokunmatik ekran için ham değerler
#define HMIN 600      // Minimum yatay (X) değeri
#define HMAX 3500     // Maksimum yatay (X) değeri  
#define VMIN 550      // Minimum dikey (Y) değeri
#define VMAX 3400     // Maksimum dikey (Y) değeri
#define XYSWAP 1      // X ve Y eksenlerini değiştir (0: kapalı, 1: açık)

// LVGL kütüphanesini ve dokunmatik ekran kütüphanesini içe aktar
#include <lvgl.h>
#include <TFT_Touch.h>

// Eğer TFT_eSPI kullanılıyorsa, ilgili kütüphaneyi içe aktar
#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
#endif

/*Ekran çözünürlüğü ve dönüş ayarları*/
#define TFT_HOR_RES   320    // Ekranın yatay çözünürlüğü (pixel)
#define TFT_VER_RES   240    // Ekranın dikey çözünürlüğü (pixel)
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0  // Ekran dönüş yönü

/*LVGL çizim buffer'ı - ekran boyutunun 1/10'u genellikle iyi çalışır. Boyut byte cinsindendir*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];  // Çizim işlemleri için buffer

// Dokunmatik ekran nesnesi oluştur (pin tanımlamaları)
TFT_Touch touch = TFT_Touch(XPT2046_CS, XPT2046_CLK, XPT2046_MOSI, XPT2046_MISO);

// Hata ayıklama için log fonksiyonu
#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);  // Seviye parametresini kullanma
    Serial.println(buf);  // Seri porta mesajı yaz
    Serial.flush();      // Buffer'ı temizle
}
#endif

/* LVGL ekranı güncellemek için bu fonksiyonu çağırır*/
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
{
    /*Burada px_map'teki verileri ekranın belirtilen alanına kopyalamanız gerekir*/
    
    /*Örnek implementasyon:
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    my_set_window(area->x1, area->y1, w, h);
    my_draw_bitmaps(px_map, w * h);
    */
    
    /*LVGL'ye hazır olduğunuzu söyleyin*/
    lv_display_flush_ready(disp);
}

/*Dokunmatik ekranı oku*/

void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
    int32_t x, y;
    bool touched = touch.Pressed();  // Dokunma durumunu kontrol et

    x = touch.X();  // X koordinatını al
    y = touch.Y();  // Y koordinatını al

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;  // Dokunma yoksa serbest bırakılmış durum
    } else {
        data->state = LV_INDEV_STATE_PRESSED;   // Dokunma varsa basılı durum
        data->point.x = x;  // X koordinatını ayarla
        data->point.y = y;  // Y koordinatını ayarla

        // Hata ayıklama için ham koordinatları seri porta yaz
      //  Serial.printf("Dokunuldu: %s - Ham X: %ld, Ham Y: %ld\n", touched ? "EVET" : "HAYIR", x, y);
    }
}

/*Zaman kaynağı olarak Arduino'nun millis() fonksiyonunu kullan*/
static uint32_t my_tick(void)
{
    return millis();  // Geçen milisaniyeyi döndür
}

lv_obj_t *button, *button2, *button3, *button4, *button5;
lv_obj_t *label,*label2, *label3, *label4, *label5;
lv_obj_t *kutu;

void setup()
{
    // GPIO pinlerini çıkış olarak ayarla ve başlangıçta HIGH yap
    pinMode(17, OUTPUT);
    digitalWrite(17, HIGH);
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    // LVGL versiyon bilgisini oluştur
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    // Seri iletişimi başlat
    Serial.begin( 115200 );
    Serial.println( LVGL_Arduino );

    // Dokunmatik ekranı kalibre et
    touch.setCal(HMIN, HMAX, VMIN, VMAX, TFT_HOR_RES, TFT_VER_RES, XYSWAP);
    touch.setRotation(3);  // Ekran dönüşünü ayarla
    lv_init();  // LVGL'yi başlat

    /*Zaman kaynağını ayarla - LVGL'nin geçen süreyi bilmesi için*/
    lv_tick_set_cb(my_tick);

    /*Hata ayıklama için yazdırma fonksiyonunu kaydet*/
#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print );
#endif

    lv_display_t * disp;  // Display nesnesi
#if LV_USE_TFT_ESPI
    /*TFT_eSPI LVGL konfigürasyonunda etkinse, ekranı basit şekilde başlat*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);
#else
    /*Değilse kendiniz bir display oluşturun*/
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

    /*Giriş aygıtı sürücüsünü başlat*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Dokunmatik ekran POINTER tipinde olmalı*/
    lv_indev_set_read_cb(indev, my_touchpad_read);

    lv_obj_t *scr = lv_screen_active(); // Aktif ekran nesnesini al SCR EKRAN KENDİSİ ==================================== //
      lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

    // ==================== BİRİNCİ BUTON (ORTA) ====================
    
    // Birinci butonu oluştur ve ekranın ortasına yerleştir
    button = lv_button_create(lv_screen_active());
    lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
    

    // Buton üzerinde etiket oluştur
    label = lv_label_create(button);
    lv_label_set_text(label, "Off - M"); // Başlangıç metni "Off"
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Buton tıklama olayını ekle - GPIO 17'yi kontrol eder
    lv_obj_add_event_cb(button, [](lv_event_t *e) 
     {
        static bool buttonState = false; // Buton durumu (false: Kapalı, true: Açık)
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e); // Tıklanan butonu al
        lv_obj_t *label = lv_obj_get_child(btn, 0); // Butonun ilk çocuğu (etiket)

        // Durumu tersine çevir (toggle)
        buttonState = !buttonState;

        if (buttonState) {
            lv_label_set_text(label, "On");   // Etiketi "On" yap
            digitalWrite(17, LOW);            // GPIO 17'yi LOW yap (aktif)
            Serial.println("Button 1: On");   // Seri porta durumu yaz
        } else {
            lv_label_set_text(label, "Off");  // Etiketi "Off" yap
            digitalWrite(17, HIGH);           // GPIO 17'yi HIGH yap (pasif)
            Serial.println("Button 1: Off");  // Seri porta durumu yaz
        }

        // Etiketi buton içinde tekrar ortala
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }, LV_EVENT_CLICKED, NULL);

    // ==================== İKİNCİ BUTON (SOL ÜST) ====================
    
    // İkinci butonu oluştur ve sol üst köşeye yerleştir
    button2 = lv_button_create(lv_screen_active());
    lv_obj_align(button2, LV_ALIGN_TOP_LEFT, 0, 0); // Sol üst köşe

    // İkinci buton için etiket oluştur
    label2 = lv_label_create(button2);
    lv_label_set_text(label2, "Off - K"); // Başlangıç metni "Off"
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0); // Etiketi buton içinde ortala

    // İkinci buton tıklama olayını ekle - GPIO 4'ü kontrol eder
    lv_obj_add_event_cb(button2, [](lv_event_t *e) 
     {
        static bool buttonState2 = false; // İkinci buton durumu
        lv_obj_t *btn2 = (lv_obj_t *)lv_event_get_target(e); // Tıklanan butonu al
        lv_obj_t *label2 = lv_obj_get_child(btn2, 0); // Butonun etiketini al

        // Durumu tersine çevir
        buttonState2 = !buttonState2;

        if (buttonState2) {
            lv_label_set_text(label2, "On");   // Etiketi "On" yap
            digitalWrite(4, LOW);              // GPIO 4'ü LOW yap (aktif)
            Serial.println("Button 2: On");    // Seri porta durumu yaz
        } else {
            lv_label_set_text(label2, "Off");  // Etiketi "Off" yap
            digitalWrite(4, HIGH);             // GPIO 4'ü HIGH yap (pasif)
            Serial.println("Button 2: Off");   // Seri porta durumu yaz
        }

        // Etiketi buton içinde tekrar ortala
        lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
    }, LV_EVENT_CLICKED, NULL);

    // ==================== ÜÇÜNCÜ BUTON (SOL ALT) ====================
    
    // Üçüncü butonu oluştur ve sol üstten 80 pixel aşağıya yerleştir
    button3 = lv_button_create(lv_screen_active());
    lv_obj_align(button3, LV_ALIGN_TOP_LEFT, 0, 208);

    // Üçüncü buton için etiket oluştur
    label3 = lv_label_create(button3);
    lv_label_set_text(label3, "Off - Y"); // Başlangıç metni "Off"
    lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0); // Etiketi buton içinde ortala

    // Üçüncü buton tıklama olayını ekle - GPIO 16'yı kontrol eder
    lv_obj_add_event_cb(button3, [](lv_event_t *e) 
     {
        static bool buttonState3 = false; // Üçüncü buton durumu
        lv_obj_t *btn3 = (lv_obj_t *)lv_event_get_target(e); // Tıklanan butonu al
        lv_obj_t *label3 = lv_obj_get_child(btn3, 0); // Butonun etiketini al

        // Durumu tersine çevir
        buttonState3 = !buttonState3;

        if (buttonState3) {
            lv_label_set_text(label3, "On");   // Etiketi "On" yap
            digitalWrite(16, LOW);             // GPIO 16'yı LOW yap (aktif)
            Serial.println("Button 3: On");    // Seri porta durumu yaz
        } else {
            lv_label_set_text(label3, "Off");  // Etiketi "Off" yap
            digitalWrite(16, HIGH);            // GPIO 16'yı HIGH yap (pasif)
            Serial.println("Button 3: Off");   // Seri porta durumu yaz
        }

        // Etiketi buton içinde tekrar ortala
        lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);
    }, LV_EVENT_CLICKED, NULL);

      // ==================== DÖRDÜNCÜ BUTON (SAĞ ALT) ====================
    
    // Üçüncü butonu oluştur ve sol üstten 208 pixel aşağıya yerleştir
    button4 = lv_button_create(lv_screen_active());
    lv_obj_align(button4, LV_ALIGN_TOP_RIGHT, 0, 208);

    // Butonun arka plan rengini kırmızı yap
    lv_obj_set_style_bg_color(button4, lv_color_hex(0xFFB347), LV_PART_MAIN);       // TURUNCU
    lv_obj_set_style_bg_color(button4, lv_color_hex(0xFFB347), LV_STATE_PRESSED);  // YEŞİL

    // Üçüncü buton için etiket oluştur
    label4 = lv_label_create(button4);
    lv_label_set_text(label4, "Off"); // Başlangıç metni "Off"
    lv_obj_align(label4, LV_ALIGN_CENTER, 0, 0); // Etiketi buton içinde ortala
    lv_obj_set_style_text_color(label4, lv_color_hex(0x0000FF), 0); // Buton İçerisindeki Yazının Rengi Mavi
    

    // Üçüncü buton tıklama olayını ekle - GPIO 16'yı kontrol eder
    lv_obj_add_event_cb(button4, [](lv_event_t *e) 
     {
        static bool buttonState4 = false; // Üçüncü buton durumu
        lv_obj_t *btn4 = (lv_obj_t *)lv_event_get_target(e); // Tıklanan butonu al
        lv_obj_t *label4 = lv_obj_get_child(btn4, 0); // Butonun etiketini al

        // Durumu tersine çevir
        buttonState4 = !buttonState4;

        if (buttonState4) {
            lv_label_set_text(label4, "On");   // Etiketi "On" yap
            lv_obj_set_style_bg_color(button4, lv_color_hex(0x00FF00), LV_PART_MAIN); // Yeşil 
            lv_label_set_text(label, "On");
            lv_label_set_text(label2, "On");
            digitalWrite(17, LOW);   
            digitalWrite(4, LOW);            // GPIO 16'yı LOW yap (aktif)
            Serial.println("Button 4: On");    // Seri porta durumu yaz
        } else {
            lv_label_set_text(label4, "Off");  // Etiketi "Off" yap
            lv_obj_set_style_bg_color(button4, lv_color_hex(0xFFB347), LV_PART_MAIN); // Kırmızı
            lv_label_set_text(label, "Off");
            lv_label_set_text(label2, "Off");
            digitalWrite(17, HIGH);            // GPIO 16'yı HIGH yap (pasif)
            digitalWrite(4, HIGH);  
            Serial.println("Button 4: Off");   // Seri porta durumu yaz
        }

        // Etiketi buton içinde tekrar ortala
        lv_obj_align(label4, LV_ALIGN_CENTER, 0, 0);
    }, LV_EVENT_CLICKED, NULL);

    
      // ==================== BEŞİNCİ BUTON (SAĞ ÜST) ====================
    
    // Üçüncü butonu oluştur ve sol üstten 208 pixel aşağıya yerleştir
    button5 = lv_button_create(lv_screen_active());
    lv_obj_align(button5, LV_ALIGN_TOP_RIGHT, 0, 0);

    // Butonun arka plan rengini kırmızı yap
    lv_obj_set_style_bg_color(button5, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_bg_color(button5, lv_color_hex(0x00FF00), LV_STATE_PRESSED);

    // Üçüncü buton için etiket oluştur
    label5 = lv_label_create(button5);
    lv_label_set_text(label5, "Off"); // Başlangıç metni "Off"
    lv_obj_align(label5, LV_ALIGN_CENTER, 0, 0); // Etiketi buton içinde ortala
    lv_obj_set_style_text_color(label5, lv_color_hex(0x000000), 0); // Buton İçerisindeki Yazının Rengi Siyah

    // Üçüncü buton tıklama olayını ekle - GPIO 16'yı kontrol eder
    lv_obj_add_event_cb(button5, [](lv_event_t *e) 
     {

        static bool buttonState5 = false; // Üçüncü buton durumu
        lv_obj_t *btn5 = (lv_obj_t *)lv_event_get_target(e); // Tıklanan butonu al
        lv_obj_t *label5 = lv_obj_get_child(btn5, 0); // Butonun etiketini al

        // Durumu tersine çevir
        buttonState5 = !buttonState5;

        if (buttonState5) {
            lv_label_set_text(label5, "On");   // Etiketi "On" yap
            lv_obj_set_style_bg_color(button5, lv_color_hex(0x00FF00), LV_PART_MAIN); // Yeşil
            lv_label_set_text(label, "On");
            lv_label_set_text(label3, "On");
            digitalWrite(17, LOW);   
            digitalWrite(16, LOW);            // GPIO 16'yı LOW yap (aktif)
            Serial.println("Button 5: On");    // Seri porta durumu yaz
        } else {
            lv_label_set_text(label5, "Off");  // Etiketi "Off" yap
            lv_obj_set_style_bg_color(button5, lv_color_hex(0xFF0000), LV_PART_MAIN); // Kırmızı
            lv_label_set_text(label, "Off");
            lv_label_set_text(label3, "Off");
            digitalWrite(17, HIGH);            // GPIO 16'yı HIGH yap (pasif)
            digitalWrite(16, HIGH);  
            Serial.println("Button 5: Off");   // Seri porta durumu yaz
        }

        // Etiketi buton içinde tekrar ortala
        lv_obj_align(label5, LV_ALIGN_CENTER, 0, 0);
    }, LV_EVENT_CLICKED, NULL);


        // ==================== Yazı Yazdırma ====================

        lv_obj_t *label6 = lv_label_create(scr); // Ekranda yazı nesnesi oluştur
        lv_label_set_text(label6, "Esp-32 Buton Ile Led Kontrolu"); // Yazı içeriği
        static lv_style_t style;    // Stil değişkeni oluştur
        lv_style_init(&style);      // Başlat
        lv_style_set_text_font(&style, &lv_font_montserrat_12); // Yazı Fontu
        lv_style_set_text_color(&style, lv_color_hex(0xFFFFFF)); // Beyaz Yazı
        lv_obj_add_style(label6, &style, 0);
        lv_obj_align(label6, LV_ALIGN_CENTER, 0, -100);

        // ==================== Kutu Çizme ====================

        // Boş bir obje oluştur (kutu olarak kullanacağız)
        kutu = lv_obj_create(lv_scr_act());
        // Ebatları 10x10 piksel yap
        lv_obj_set_size(kutu, 10, 10);

        lv_obj_align(kutu, LV_ALIGN_RIGHT_MID, -10, 0);

        // Dilerseniz konumunu belirleyin, örneğin ekranın ortasına
        //lv_obj_center(kutu);
        // Rengini ayarlayın (örneğin mavi arka plan)
        lv_obj_set_style_bg_color(kutu, lv_color_hex(0x0000FF), LV_PART_MAIN);
        // Kenarlık rengini ve kalınlığını ayarlayın
        lv_obj_set_style_border_color(kutu, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_border_width(kutu, 2, LV_PART_MAIN);

    // >>>>>------------Kayar Label Oluşturma Tam Orta----------------------------------------<<<<<

   // --- GÖRÜNMEYİ SINIRLAYAN ŞEFFAF KAP (PENCERE) ---
   lv_obj_t *window = lv_obj_create(scr);
   lv_obj_set_size(window, 95, 30); // Yazının görüneceği alan boyutu
   lv_obj_align(window, LV_ALIGN_CENTER, 0, 55); // Tam Orta 55 Aşşağı

   // Şeffaf yap ama kesme özelliğini aktif et
   lv_obj_set_style_bg_opa(window, LV_OPA_TRANSP, 0); // Şeffaf arkaplan
   lv_obj_set_style_border_opa(window, LV_OPA_TRANSP, 0); // Şeffaf kenarlık
   lv_obj_set_style_pad_all(window, 0, 0); // Padding yok
   lv_obj_add_flag(window, LV_OBJ_FLAG_OVERFLOW_VISIBLE); // Taşmayı gizle (ÖNEMLI!)
   lv_obj_clear_flag(window, LV_OBJ_FLAG_SCROLLABLE); // Kaydırma yok

   // --- DAİRESEL KAYAN YAZI ---
   lv_obj_t *scrolling_label = lv_label_create(window);
   lv_label_set_long_mode(scrolling_label, LV_LABEL_LONG_SCROLL_CIRCULAR); // Dairesel kayma
   lv_label_set_text(scrolling_label, "H.M.C    ");
   lv_obj_set_style_text_color(scrolling_label, lv_color_hex(0x00FF00), 0); // Yeşil yazı
   lv_obj_set_style_text_font(scrolling_label, &lv_font_montserrat_30, 0);

   // Yazıyı pencere genişliğine ayarla
   lv_obj_set_width(scrolling_label, 95); // Pencere boyutu kadar
   lv_obj_align(scrolling_label, LV_ALIGN_LEFT_MID, 0, 0); // Sola hizala
   lv_obj_set_style_anim_time(scrolling_label, 7000, LV_PART_MAIN); // 7 saniyede bir tur Animasyon hızını ayarla (ms cinsinden)



    Serial.println( "Setup done" ); // Kurulum tamamlandı mesajı
}

void loop()
{
    lv_timer_handler(); /* GUI'nin çalışması için timer'ı işle */    
    delay(5); /* 5ms bekle - sistemin nefes alması için */
}