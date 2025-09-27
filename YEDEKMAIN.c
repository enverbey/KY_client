#include <Arduino.h>
#include <LovyanGFX.hpp>

// Professional Design System - Modern Color Palette
#define PRIMARY_BG    0x0F1419  // Deep space blue
#define SURFACE_BG    0x1A1F2B  // Card background
#define SURFACE_HOVER 0x252A38  // Hover state
#define ACCENT_CYAN   0x00D9FF  // Vibrant cyan
#define ACCENT_PURPLE 0x7C3AED  // Modern purple
#define SUCCESS_MINT  0x00F5A0  // Fresh mint
#define WARNING_AMBER 0xFFB020  // Warm amber
#define DANGER_CORAL  0xFF6B6B  // Soft coral
#define TEXT_PRIMARY  0xF8FAFC  // Pure white text
#define TEXT_SECONDARY 0x94A3B8 // Muted text
#define BORDER_SUBTLE 0x334155  // Subtle borders
#define SHADOW_DARK   0x0A0E14  // Deep shadows
#define GLOW_CYAN     0x22D3EE  // Cyan glow

// Responsive Layout System
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 320;

// Adaptive Layout Constants
const int HEADER_HEIGHT = 45;        // Kompakt header
const int CARD_HEIGHT = 72;          // Optimal touch target
const int CARD_MARGIN = 12;          // Balanced spacing
const int CARD_RADIUS = 16;          // Modern rounded corners
const int ICON_SIZE = 48;            // Perfect icon size
const int SPACING_XS = 4;
const int SPACING_SM = 8;
const int SPACING_MD = 12;
const int SPACING_LG = 16;
const int SPACING_XL = 24;

enum State {
  MENU_SCREEN,
  SELECTED_SCREEN
};

enum ContainerType {
  SMALL = 0,
  MEDIUM = 1, 
  LARGE = 2
};

State currentState = MENU_SCREEN;
ContainerType selectedContainer = SMALL;

// TFT ve Touch konfigürasyonu
class LGFX : public lgfx::LGFX_Device {
public:
  LGFX() {
    auto bus = new lgfx::Bus_SPI();
    {
      auto cfg = bus->config();
      cfg.spi_host = VSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = 1;
      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = 12;
      cfg.pin_dc = 2;
      bus->config(cfg);
    }

    auto panel = new lgfx::Panel_ILI9341();
    {
      auto cfg = panel->config();
      cfg.pin_cs = 15;
      cfg.pin_rst = -1;
      cfg.pin_busy = -1;
      cfg.memory_width = 240;
      cfg.memory_height = 320;
      cfg.panel_width = 240;
      cfg.panel_height = 320;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      panel->config(cfg);
    }
    panel->setBus(bus);
    setPanel(panel);

    // Touch konfigürasyonu - DÜZELTİLMİŞ
    auto touch = new lgfx::Touch_XPT2046();
    {
      auto cfg = touch->config();
      cfg.spi_host = HSPI_HOST;
      cfg.freq = 1000000;
      cfg.pin_sclk = 25;
      cfg.pin_mosi = 32;
      cfg.pin_miso = 39;
      cfg.pin_cs = 33;
      cfg.pin_int = 36;
      cfg.offset_rotation = 0;
      // Touch kalibrasyon değerleri düzeltildi
      cfg.x_min = 320;
      cfg.x_max = 3700;
      cfg.y_min = 400;  
      cfg.y_max = 3800;
      cfg.bus_shared = true;
      touch->config(cfg);
    }
    panel->setTouch(touch);
  }
};

LGFX display;

// Gelişmiş touch kontrolü
struct TouchState {
  bool isPressed;
  bool wasPressed;
  int x, y;
  int lastX, lastY;
  unsigned long pressTime;
  unsigned long lastTouchTime;
  bool validTouch;
} touch;

// Professional Container Card Structure
struct ContainerCard {
  int x, y, w, h;
  ContainerType type;
  const char* title;
  const char* subtitle;
  const char* capacity;
  const char* dimensions;
  uint16_t accentColor;
  uint16_t glowColor;
  const char* icon;  // Unicode character for icon
};

ContainerCard cards[3];

// Touch okuma - GELIŞTIRILMIŞ VE DÜZELTİLMİŞ
bool readTouch() {
  uint16_t rawX, rawY;
  bool isTouching = display.getTouch(&rawX, &rawY);
  unsigned long now = millis();
  
  if (isTouching) {
    // Çoklu okuma ile stabilite
    uint16_t readings[3][2];
    int validCount = 0;
    
    for (int i = 0; i < 3; i++) {
      uint16_t tx, ty;
      if (display.getTouch(&tx, &ty)) {
        readings[validCount][0] = tx;
        readings[validCount][1] = ty;
        validCount++;
      }
      delay(2);
    }
    
    if (validCount < 2) return false;
    
    // Ortalama al
    uint32_t sumX = 0, sumY = 0;
    for (int i = 0; i < validCount; i++) {
      sumX += readings[i][0];
      sumY += readings[i][1];
    }
    
    // Touch koordinatları - Y eksenini ters çevir (ters montaj için)
    touch.x = sumX / validCount;
    touch.y = SCREEN_HEIGHT - (sumY / validCount);  // Y koordinatını ters çevir
    
    // Ekran sınırları kontrolü
    if (touch.x < 0 || touch.x >= SCREEN_WIDTH || 
        touch.y < 0 || touch.y >= SCREEN_HEIGHT) {
      return false;
    }
    
    // İlk basış
    if (!touch.wasPressed) {
      touch.isPressed = true;
      touch.wasPressed = true;
      touch.pressTime = now;
      touch.lastX = touch.x;
      touch.lastY = touch.y;
      touch.validTouch = true;
      return false;
    }
    
    // Çok fazla hareket varsa iptal et
    int deltaX = abs(touch.x - touch.lastX);
    int deltaY = abs(touch.y - touch.lastY);
    if (deltaX > 20 || deltaY > 20) {
      touch.validTouch = false;
    }
    
  } else {
    // Bırakma
    if (touch.wasPressed && touch.validTouch) {
      touch.wasPressed = false;
      touch.isPressed = false;
      
      // Minimum basış süresi kontrolü (50ms-500ms arası)
      unsigned long pressDuration = now - touch.pressTime;
      if (pressDuration > 50 && pressDuration < 500) {
        touch.lastTouchTime = now;
        return true; // Geçerli tap
      }
    }
    
    touch.wasPressed = false;
    touch.isPressed = false;
    touch.validTouch = true;
  }
  
  return false;
}

// Advanced Visual Effects
void drawVerticalGradient(int x, int y, int w, int h, uint16_t topColor, uint16_t bottomColor) {
  for (int i = 0; i < h; i++) {
    float ratio = (float)i / h;
    
    uint8_t r1 = (topColor >> 11) & 0x1F;
    uint8_t g1 = (topColor >> 5) & 0x3F;
    uint8_t b1 = topColor & 0x1F;
    
    uint8_t r2 = (bottomColor >> 11) & 0x1F;
    uint8_t g2 = (bottomColor >> 5) & 0x3F;
    uint8_t b2 = bottomColor & 0x1F;
    
    uint8_t r = r1 + (r2 - r1) * ratio;
    uint8_t g = g1 + (g2 - g1) * ratio;
    uint8_t b = b1 + (b2 - b1) * ratio;
    
    uint16_t color = (r << 11) | (g << 5) | b;
    display.drawFastHLine(x, y + i, w, color);
  }
}

void drawGlowEffect(int x, int y, int w, int h, int radius, uint16_t glowColor) {
  for (int i = 1; i <= 3; i++) {
    uint8_t alpha = 80 / i;
    uint16_t fadeColor = display.color565(
      ((glowColor >> 11) & 0x1F) * alpha / 255,
      ((glowColor >> 5) & 0x3F) * alpha / 255,
      (glowColor & 0x1F) * alpha / 255
    );
    display.drawRoundRect(x - i, y - i, w + 2*i, h + 2*i, radius + i, fadeColor);
  }
}

// Professional Header with Subtle Animation
void drawModernHeader(const char* title, const char* subtitle = nullptr) {
  // Background gradient
  drawVerticalGradient(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, PRIMARY_BG, SURFACE_BG);
  
  // Accent line at bottom
  for (int i = 0; i < 3; i++) {
    uint16_t lineColor = display.color565(0, 217 - i*40, 255 - i*40);
    display.drawFastHLine(0, HEADER_HEIGHT - 3 + i, SCREEN_WIDTH, lineColor);
  }
  
  // Title
  display.setFont(&fonts::Font2);
  display.setTextColor(TEXT_PRIMARY);
  display.setTextDatum(textdatum_t::middle_center);
  
  if (subtitle) {
    display.drawString(title, SCREEN_WIDTH/2, HEADER_HEIGHT/2 - 8);
    display.setFont(&fonts::Font0);
    display.setTextColor(TEXT_SECONDARY);
    display.drawString(subtitle, SCREEN_WIDTH/2, HEADER_HEIGHT/2 + 8);
  } else {
    display.drawString(title, SCREEN_WIDTH/2, HEADER_HEIGHT/2);
  }
}

// Modern 3D Container Icon with Depth
void drawModernContainerIcon(int x, int y, int size, uint16_t accentColor, bool isSelected = false) {
  // Base container body with 3D effect
  int bodyW = size * 0.8;
  int bodyH = size * 0.5;
  int bodyX = x + (size - bodyW) / 2;
  int bodyY = y + size * 0.3;
  
  // Shadow/depth effect
  display.fillRoundRect(bodyX + 2, bodyY + 2, bodyW, bodyH, 4, SHADOW_DARK);
  
  // Main body gradient
  drawVerticalGradient(bodyX, bodyY, bodyW, bodyH, accentColor, 
                      display.color565(
                        ((accentColor >> 11) & 0x1F) * 0.6,
                        ((accentColor >> 5) & 0x3F) * 0.6,
                        (accentColor & 0x1F) * 0.6
                      ));
  
  // Border
  display.drawRoundRect(bodyX, bodyY, bodyW, bodyH, 4, TEXT_PRIMARY);
  
  // Container door
  int doorX = bodyX + bodyW * 0.7;
  int doorY = bodyY + 4;
  int doorH = bodyH - 8;
  display.drawFastVLine(doorX, doorY, doorH, TEXT_PRIMARY);
  display.drawFastVLine(doorX + 1, doorY, doorH, TEXT_SECONDARY);
  
  // Door handle
  display.fillCircle(doorX - 6, bodyY + bodyH/2, 2, TEXT_PRIMARY);
  
  // Container legs/supports
  display.drawFastHLine(bodyX - 3, bodyY + bodyH, bodyW + 6, BORDER_SUBTLE);
  display.drawFastHLine(bodyX - 2, bodyY + bodyH + 1, bodyW + 4, BORDER_SUBTLE);
  
  // Glow effect when selected
  if (isSelected) {
    drawGlowEffect(bodyX - 2, bodyY - 2, bodyW + 4, bodyH + 4, 6, accentColor);
  }
  
  // Size indicator dots
  int dotY = y + size * 0.85;
  int dotSize = size == SMALL ? 1 : (size == MEDIUM ? 2 : 3);
  for (int i = 0; i < dotSize; i++) {
    display.fillCircle(x + size/2 - dotSize + i*3, dotY, 1, accentColor);
  }
}

// Ultra-Modern Card Design with Glassmorphism Effect
void drawProfessionalCard(const ContainerCard& card, bool isPressed = false, bool isHovered = false) {
  // Dynamic colors based on state
  uint16_t bgColor = isPressed ? SURFACE_HOVER : SURFACE_BG;
  uint16_t borderColor = isPressed ? card.accentColor : BORDER_SUBTLE;
  
  // Multi-layer shadow for depth
  for (int i = 0; i < 4; i++) {
    uint8_t shadowAlpha = 30 - i * 5;
    uint16_t shadowColor = display.color565(shadowAlpha/8, shadowAlpha/8, shadowAlpha/8);
    display.fillRoundRect(card.x + i + 2, card.y + i + 2, card.w, card.h, CARD_RADIUS, shadowColor);
  }
  
  // Main card background with subtle gradient
  drawVerticalGradient(card.x, card.y, card.w, card.h, bgColor, 
                      display.color565(
                        ((bgColor >> 11) & 0x1F) + 1,
                        ((bgColor >> 5) & 0x3F) + 2,
                        (bgColor & 0x1F) + 1
                      ));
  
  // Animated border
  if (isPressed) {
    for (int i = 0; i < 2; i++) {
      display.drawRoundRect(card.x - i, card.y - i, card.w + 2*i, card.h + 2*i, 
                           CARD_RADIUS + i, card.accentColor);
    }
  } else {
    display.drawRoundRect(card.x, card.y, card.w, card.h, CARD_RADIUS, borderColor);
  }
  
  // Icon area with subtle background
  int iconAreaX = card.x + SPACING_MD;
  int iconAreaY = card.y + (card.h - ICON_SIZE) / 2;
  int iconBgSize = ICON_SIZE + SPACING_SM;
  
  // Icon background circle
  display.fillCircle(iconAreaX + iconBgSize/2, iconAreaY + iconBgSize/2, iconBgSize/2, 
                    display.color565(
                      ((card.accentColor >> 11) & 0x1F) * 0.3,
                      ((card.accentColor >> 5) & 0x3F) * 0.3,
                      (card.accentColor & 0x1F) * 0.3
                    ));
  
  // Modern container icon
  drawModernContainerIcon(iconAreaX + SPACING_XS, iconAreaY + SPACING_XS, 
                         ICON_SIZE - SPACING_SM, card.accentColor, isPressed);
  
  // Typography hierarchy
  int textX = iconAreaX + iconBgSize + SPACING_MD;
  int textY = card.y + SPACING_MD;
  
  // Title - Bold and prominent
  display.setFont(&fonts::Font2);
  display.setTextColor(TEXT_PRIMARY);
  display.setTextDatum(textdatum_t::baseline_left);
  display.drawString(card.title, textX, textY + 16);
  
  // Subtitle - Medium weight
  display.setFont(&fonts::Font0);
  display.setTextColor(TEXT_SECONDARY);
  display.drawString(card.subtitle, textX, textY + 32);
  
  // Capacity info with accent color
  display.setTextColor(card.accentColor);
  display.drawString(card.capacity, textX, textY + 46);
  
  // Dimensions - Small and subtle
  display.setFont(&fonts::Font0);
  display.setTextColor(TEXT_SECONDARY);
  display.drawString(card.dimensions, textX, textY + 58);
  
  // Modern chevron indicator
  int chevronX = card.x + card.w - SPACING_LG;
  int chevronY = card.y + card.h / 2;
  uint16_t chevronColor = isPressed ? card.accentColor : TEXT_SECONDARY;
  
  // Animated chevron
  for (int i = 0; i < 3; i++) {
    display.drawPixel(chevronX - 6 + i*2, chevronY - 4 + i, chevronColor);
    display.drawPixel(chevronX - 6 + i*2, chevronY + 4 - i, chevronColor);
  }
  
  // Status indicator (tiny dot)
  display.fillCircle(card.x + card.w - SPACING_XS, card.y + SPACING_XS, 2, card.accentColor);
}

// Initialize Professional Container Cards
void initializeProfessionalCards() {
  int cardWidth = SCREEN_WIDTH - (2 * CARD_MARGIN);
  int startY = HEADER_HEIGHT + SPACING_LG;
  int cardSpacing = CARD_HEIGHT + SPACING_MD;
  
  cards[0] = {
    CARD_MARGIN, startY, cardWidth, CARD_HEIGHT,
    SMALL, "Kompakt", "20ft Standard", "TEU 1.0", "6.1m × 2.4m × 2.6m",
    0x07E0, 0x07FF, "□"  // Green and Cyan in RGB565
  };
  
  cards[1] = {
    CARD_MARGIN, startY + cardSpacing, cardWidth, CARD_HEIGHT,
    MEDIUM, "Standart", "40ft Standard", "TEU 2.0", "12.2m × 2.4m × 2.6m", 
    0xFE00, 0x07FF, "▬"  // Orange and Cyan in RGB565
  };
  
  cards[2] = {
    CARD_MARGIN, startY + 2*cardSpacing, cardWidth, CARD_HEIGHT,
    LARGE, "High Cube", "40ft High Cube", "TEU 2.25", "12.2m × 2.4m × 2.9m",
    0xF81F, 0x07FF, "■"  // Magenta and Cyan in RGB565
  };
}

// Modern Menu Screen with Professional Layout
void drawMenuScreen() {
  display.fillScreen(PRIMARY_BG);
  drawModernHeader("Container Selection", "Choose your shipping solution");
  
  initializeProfessionalCards();
  
  for (int i = 0; i < 3; i++) {
    drawProfessionalCard(cards[i]);
  }
  
  // Bottom info bar with glassmorphism
  int infoY = SCREEN_HEIGHT - 35;
  display.fillRoundRect(SPACING_MD, infoY, SCREEN_WIDTH - 2*SPACING_MD, 25, 12, SURFACE_BG);
  display.drawRoundRect(SPACING_MD, infoY, SCREEN_WIDTH - 2*SPACING_MD, 25, 12, BORDER_SUBTLE);
  
  display.setFont(&fonts::Font0);
  display.setTextColor(TEXT_SECONDARY);
  display.setTextDatum(textdatum_t::middle_center);
  display.drawString("Tap to select container type", SCREEN_WIDTH/2, infoY + 12);
}

// Enhanced Selection Screen with Animation
void drawSelectedScreen() {
  display.fillScreen(PRIMARY_BG);
  drawModernHeader("Selection Confirmed", "Your chosen container");
  
  const ContainerCard& selectedCard = cards[selectedContainer];
  
  // Selection confirmation message
  display.setFont(&fonts::Font2);
  display.setTextColor(TEXT_PRIMARY);
  display.setTextDatum(textdatum_t::middle_center);
  display.drawString("Selected Container", SCREEN_WIDTH/2, 85);
  
  // Large featured card with glow
  int bigCardW = SCREEN_WIDTH - 2*SPACING_LG;
  int bigCardH = 100;
  int bigCardX = SPACING_LG;
  int bigCardY = 105;
  
  // Multi-layer glow effect
  drawGlowEffect(bigCardX - 4, bigCardY - 4, bigCardW + 8, bigCardH + 8, CARD_RADIUS + 4, selectedCard.accentColor);
  
  // Featured card background
  drawVerticalGradient(bigCardX, bigCardY, bigCardW, bigCardH, selectedCard.accentColor, SURFACE_BG);
  display.drawRoundRect(bigCardX, bigCardY, bigCardW, bigCardH, CARD_RADIUS, selectedCard.accentColor);
  
  // Large icon
  drawModernContainerIcon(bigCardX + SPACING_LG, bigCardY + SPACING_LG, 60, TEXT_PRIMARY, true);
  
  // Enhanced typography
  int textX = bigCardX + 80;
  display.setFont(&fonts::Font2);
  display.setTextColor(TEXT_PRIMARY);
  display.setTextDatum(textdatum_t::baseline_left);
  display.drawString(selectedCard.title, textX, bigCardY + 25);
  
  display.setFont(&fonts::Font0);
  display.setTextColor(TEXT_SECONDARY);
  display.drawString(selectedCard.subtitle, textX, bigCardY + 40);
  
  display.setTextColor(TEXT_PRIMARY);
  display.drawString(selectedCard.capacity, textX, bigCardY + 55);
  
  display.setTextColor(TEXT_SECONDARY);
  display.drawString(selectedCard.dimensions, textX, bigCardY + 70);
  
  // Modern action button
  int btnY = SCREEN_HEIGHT - 80;
  int btnH = 50;
  int btnW = SCREEN_WIDTH - 2*SPACING_XL;
  int btnX = SPACING_XL;
  
  // Button with gradient and glow
  drawVerticalGradient(btnX, btnY, btnW, btnH, 0xF800, 0xC000);  // Red gradient
  display.drawRoundRect(btnX, btnY, btnW, btnH, 12, 0xF800);
  
  display.setFont(&fonts::Font2);
  display.setTextColor(TEXT_PRIMARY);
  display.setTextDatum(textdatum_t::middle_center);
  display.drawString("← BACK TO MENU", SCREEN_WIDTH/2, btnY + btnH/2);
}

// Touch işleme
bool isPointInRect(int px, int py, int rx, int ry, int rw, int rh) {
  return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}

void handleMenuTouch() {
  // Debug için touch koordinatları yazdır
  Serial.printf("Touch koordinatlari: (%d, %d)\n", touch.x, touch.y);
  
  for (int i = 0; i < 3; i++) {
    Serial.printf("Kart %d: x=%d, y=%d, w=%d, h=%d\n", i, cards[i].x, cards[i].y, cards[i].w, cards[i].h);
    
    if (isPointInRect(touch.x, touch.y, cards[i].x, cards[i].y, cards[i].w, cards[i].h)) {
      
      // Basış efekti göster
      drawProfessionalCard(cards[i], true);
      delay(150);
      
      // Doğru konteyner tipini seç
      selectedContainer = (ContainerType)i;  // Cast to enum
      currentState = SELECTED_SCREEN;
      drawSelectedScreen();
      
      Serial.printf("Secildi: %s (tip: %d)\n", cards[i].title, selectedContainer);
      return;
    }
  }
  
  Serial.println("Hicbir kart tiklandi");
}

void handleSelectedTouch() {
  int btnY = SCREEN_HEIGHT - 80;
  int btnH = 50;
  int btnX = SPACING_XL;
  int btnW = SCREEN_WIDTH - 2*SPACING_XL;
  
  Serial.printf("Selected touch: (%d, %d), Buton: x=%d, y=%d, w=%d, h=%d\n", 
                touch.x, touch.y, btnX, btnY, btnW, btnH);
  
  if (isPointInRect(touch.x, touch.y, btnX, btnY, btnW, btnH)) {
    // Buton basış efekti
    display.fillRoundRect(btnX, btnY, btnW, btnH, 12, 0x8000);
    delay(100);
    
    currentState = MENU_SCREEN;
    drawMenuScreen();
    Serial.println("Iptal edildi - Ana menuye donuluyor");
  } else {
    Serial.println("Iptal butonu disindan basildi");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Backlight
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);
  
  // Display init
  display.init();
  display.setRotation(0);
  
  // Touch init
  touch = {false, false, 0, 0, 0, 0, 0, 0, true};
  
  // İlk ekran
  drawMenuScreen();
  currentState = MENU_SCREEN;
  
  Serial.println("Professional Container UI Started");
}

void loop() {
  if (readTouch()) {
    Serial.printf("Gecerli touch: (%d, %d) - Durum: %s\n", 
                  touch.x, touch.y, 
                  currentState == MENU_SCREEN ? "MENU" : "SELECTED");
    
    if (currentState == MENU_SCREEN) {
      handleMenuTouch();
    } else {
      handleSelectedTouch();
    }
    
    // Çok hızlı basışları önle
    delay(20);
  }
  
  delay(10);
}