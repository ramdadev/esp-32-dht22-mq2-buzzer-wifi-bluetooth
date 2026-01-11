---

# 🔌 Wiring Schema ESP32 (MQ-2, DHT22, Buzzer)

Dokumen ini menjelaskan konfigurasi wiring antara **ESP32** dengan sensor **MQ-2**, **DHT22**, dan **Buzzer**.
Pastikan semua koneksi terpasang dengan benar sebelum menyalakan rangkaian.

---

## 🧰 Komponen yang Digunakan

* ESP32
* Sensor Gas MQ-2
* Sensor Suhu & Kelembaban DHT22
* Buzzer
* Kabel jumper

---

## 🔗 Skema Wiring

### 1️⃣ MQ-2 Gas Sensor

| MQ-2 Pin | ESP32 Pin    |
| -------- | ------------ |
| VCC      | VCC ESP32    |
| GND      | 5V ESP32     |
| AO       | GPIO34 (D34) |

---

### 2️⃣ DHT22 Sensor

| DHT22 Pin | ESP32 Pin    |
| --------- | ------------ |
| NEGATIVE  | GND          |
| OUT       | GPIO32 (D32) |
| POSITIVE  | 3.3V         |

---

### 3️⃣ Buzzer

| Buzzer Pin | ESP32 Pin    |
| ---------- | ------------ |
| POSITIVE   | GPIO25 (D25) |
| NEGATIVE   | GND          |

---

## ⚠️ Catatan Penting

* Pastikan **tegangan sesuai** dengan spesifikasi masing-masing komponen.
* GPIO **34** hanya mendukung **input (ADC)**, cocok untuk output analog MQ-2.
* Gunakan **resistor pull-up** jika diperlukan pada DHT22.
* Periksa kembali wiring sebelum upload program untuk menghindari kerusakan komponen.

---

## 📌 Penutup

Semoga dokumentasi wiring ini membantu dalam pengembangan proyek ESP32 Anda.
Jika ada tambahan sensor atau perubahan konfigurasi, silakan sesuaikan README ini.

**Terima kasih 🙏**

---
