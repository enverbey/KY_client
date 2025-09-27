#include "best_wifi.hpp"
#include <Arduino.h>
#include <esp_wifi.h>
#include <mdns.h>

// lwIP headerları
#include "lwip/icmp.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/pbuf.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"

// C++ atomic için
#include <atomic>

void loginToCaptivePortal();

extern	std::atomic<unsigned long> last_heart_beat;
static struct raw_pcb *icmp_pcb = NULL;

BestWiFi::BestWiFi(const String& ssid, const String& password)
	: _ssid(ssid), _password(password), _mdnsName("") {}

BestWiFi::~BestWiFi() {}

static u8_t icmp_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr) {
    if (!p) return 0;

    // En azından IP + ICMP header sığmalı
    if (p->tot_len < (sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
        return 0;
    }

    struct ip_hdr iphdr;
    pbuf_copy_partial(p, &iphdr, sizeof(iphdr), 0);

    // 2) IP header uzunluğunu byte cinsinden hesapla
    u16_t iphdr_len = IPH_HL(&iphdr) * 4;  // IHL (32-bit word) → byte

    // Güvenlik: toplam uzunluk yeterli mi?
    if (p->tot_len < (iphdr_len + sizeof(struct icmp_echo_hdr))) {
        return 0;
    }

    struct icmp_echo_hdr iecho;
    pbuf_copy_partial(p, &iecho, sizeof(iecho), iphdr_len);

    if (iecho.type == ICMP_ECHO){
		Serial.printf("✅ Heartbeat Alındı. Son ping'den bu yana geçen süre: %lu ms\n", (millis() - last_heart_beat.load()));
		last_heart_beat.store(millis());
    }
	else {
		Serial.println("PING GELDİ AMA ICMP HATASI VAR");
	}
    last_heart_beat.store(millis());
    return 0;
}

void icmp_listen_init(void) {
    if (icmp_pcb == NULL) {
		icmp_pcb = raw_new(IP_PROTO_ICMP);
		if (icmp_pcb != NULL) {
			raw_recv(icmp_pcb, icmp_recv, NULL);
			raw_bind(icmp_pcb, IP_ADDR_ANY);
			printf("ICMP listener başlatıldı\n");
		}
    }
}

void BestWiFi::setupWiFiEventHandlers() {
	WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
		switch (event)
		{
			case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			{
				mdns_init();
				_mdnsName = "Module_" + WiFi.macAddress();
				mdns_hostname_set(_mdnsName.c_str());
				if (this->_displayLog == ALL)
					Serial.println("📶 IP adresi alındı - Bağlantı tamamlandı");
				icmp_listen_init();
				loginToCaptivePortal();
				break;
			}
			case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			{
				if (this->_displayLog != NO_LOG)
				{
					Serial.println("⚠️  WiFi bağlantısı kesildi");
					Serial.println("🔄 Yeniden bağlanma başlatılıyor...");
				}
				if (icmp_pcb != NULL) {
                    raw_remove(icmp_pcb);
                    icmp_pcb = NULL; // Sildikten sonra pointer'ı NULL'a eşitle
                }
				break;
			}
			case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			{
				if (this->_displayLog == ALL)
					Serial.println("🔗 WiFi ağına bağlanıldı");
				break;
			}
			default:
				break;
		}
	});
}

void BestWiFi::setWaitToConnect(uint8_t flag) {
	this->__WAITTOCONNECT__ = flag;
}

void BestWiFi::displayNetworkInfo() {
	Serial.println("========================");
	Serial.println("📱 BAĞLANTI BİLGİLERİ");
	Serial.println("========================");
	Serial.print("🌐 IP Adresi: ");
	Serial.println(WiFi.localIP());
	Serial.print("🏠 Gateway: ");
	Serial.println(WiFi.gatewayIP());
	Serial.print("🛡️ MDNS HOST NAME: ");
	Serial.println(_mdnsName);
	Serial.print("🔍 DNS: ");
	Serial.println(WiFi.dnsIP());
	Serial.print("📡 Ağ Adı (SSID): ");
	Serial.println(WiFi.SSID());
	Serial.print("📶 Sinyal Gücü: ");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm");
	Serial.print("🔐 Şifreleme: ");
	Serial.println(WiFi.encryptionType(0));
	Serial.print("📍 MAC Adresi: ");
	Serial.println(WiFi.macAddress());
	Serial.print("⚡ TX Güç: ");
	Serial.println("19.5 dBm (Maksimum)");
	Serial.println("========================");
}

bool BestWiFi::connect() {
	if (this->_displayLog == ALL) 
		Serial.println("=== GÜÇLÜ IoT WiFi BAŞLATILIYOR ===");

	setupWiFiEventHandlers();
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(true);
	WiFi.begin(_ssid.c_str(), _password.c_str());
	uint8_t connectionAttempts = 0;

	while (WiFi.status() != WL_CONNECTED && (__WAITTOCONNECT__ || connectionAttempts < _maxAttempts))
	{
		delay(500);
		Serial.print("Wifi Bağlantısı Bekleniyor\n");
		connectionAttempts++;
	}
	configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

	if (WiFi.status() == WL_CONNECTED) {
		if (this->_displayLog == ALL) {
			Serial.println();
			Serial.println("✓ WiFi BAĞLANTISI BAŞARILI!");
			displayNetworkInfo();
		}
		return true;
	}
	else {
		if (this->_displayLog != NO_LOG) {
			Serial.println();
			Serial.println("✗ WiFi BAĞLANTISI BAŞARISIZ!");
		}
		return false;
	}
}
