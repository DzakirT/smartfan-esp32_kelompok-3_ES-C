# Smart Fan Hemat Energi Berbasis Deteksi Suhu dan Kehadiran Pengguna Menggunakan FSM

## Tim Pengembang

Kelompok 3 Embedded System C

- M Dzakir Thuha Al Ghaffar
- Abdillah Abyan Ilman Nafian
- Sang Pimandu Banjaran
- Syarief Choirul Anwar
- Raden Amardyena Muhammad

Fakultas Ilmu Komputer Universitas Brawijaya

## Deskripsi
Smart Fan merupakan sistem Embedded System berbasis ESP32 yang dirancang untuk menghemat energi dengan mendeteksi keberadaan pengguna dan kondisi suhu lingkungan.

### Teknologi yang Digunakan
- ESP32 DevKit V1
- PIR AM312
- DHT22
- Potensiometer 10KΩ
- LCD 16x2 I2C (PCF8574)
- Driver Motor L298N
- Motor DC
- Finite State Machine (FSM)
- Deep Sleep & Power Switching

---

## Fitur Utama
- Deteksi kehadiran pengguna menggunakan PIR AM312
- Pengaturan kecepatan kipas otomatis berdasarkan suhu ruangan
- Pengaturan kecepatan kipas manual menggunakan potensiometer
- Tampilan informasi sistem pada LCD 16x2
- Implementasi Finite State Machine (FSM)
- Deep Sleep untuk penghematan energi
- Power Switching untuk mematikan sensor yang tidak digunakan

---

## Finite State Machine
### StateSleep
- ESP32 masuk Deep Sleep
- Motor berhenti
- DHT22 dan Potensiometer dimatikan

### StateValidasi
- Validasi gerakan selama 3 detik

### ActiveAutoState
- Kecepatan kipas ditentukan oleh suhu ruangan

### ActiveManualState
- Kecepatan kipas ditentukan oleh potensiometer

---

## Komponen

| No | Komponen | Jumlah |
|----|----------|---------|
| 1 | ESP32 DevKit V1 | 1 |
| 2 | PIR AM312 | 1 |
| 3 | DHT22 | 1 |
| 4 | Potensiometer 10KΩ | 1 |
| 5 | LCD 16x2 I2C | 1 |
| 6 | Driver Motor L298N | 1 |
| 7 | Motor DC | 1 |
| 8 | Breadboard | 1 |
| 9 | Kabel Jumper | Secukupnya |

---

## Tabel Wiring

| Komponen | Pin Komponen | Pin ESP32 | Fungsi |
|-----------|-------------|-----------|---------|
| PIR AM312 | OUT | GPIO 14 | Deteksi gerakan & Wake Up |
| PIR AM312 | VCC | 3.3V | Catu daya |
| PIR AM312 | GND | GND | Ground |
| DHT22 | DATA | GPIO 4 | Pembacaan suhu |
| DHT22 | VCC | GPIO 19 | Power Switching |
| DHT22 | GND | GND | Ground |
| Potensiometer | SIG | GPIO 34 | ADC Mode Manual |
| Potensiometer | VCC | GPIO 18 | Power Switching |
| Potensiometer | GND | GND | Ground |
| LCD I2C | SDA | GPIO 21 | I2C Data |
| LCD I2C | SCL | GPIO 22 | I2C Clock |
| LCD I2C | VCC | 5V | Catu daya |
| LCD I2C | GND | GND | Ground |
| L298N | ENA | GPIO 25 | PWM Motor |
| L298N | IN1 | GPIO 26 | Kontrol Motor |
| L298N | IN2 | GPIO 27 | Kontrol Motor |
| L298N | GND | GND | Ground Bersama |
| Motor DC | OUT1 | L298N OUT1 | Terminal Motor 1 |
| Motor DC | OUT2 | L298N OUT2 | Terminal Motor 2 |

---

## Cara Menjalankan
1. Install Arduino IDE
2. Install ESP32 Board Package
3. Install library (DHT Sensor Library, Adafruit Unified Sensor, LiquidCrystal_I2C)
4. Upload program ke ESP32
5. Hubungkan komponen sesuai tabel wiring
6. Nyalakan sistem

---

## Implementasi Hemat Energi
Ketika tidak ada aktivitas pengguna selama 60 detik
- ESP32 masuk mode Deep Sleep
- Motor DC berhenti
- Sensor DHT22 dimatikan
- Potensiometer dimatikan
- PIR tetap aktif sebagai wake-up source
