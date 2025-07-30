// TD4 クロックジェネレーター & ROMエミュレーター

// クロックジェネレーター用のピン
#define CLOCK_OUTPUT_PIN    2
#define RESET_OUTPUT_PIN    3

// ROMへのアドレス入力用のピン
#define ADDR_BIT0_PIN       6   // LSB
#define ADDR_BIT1_PIN       7
#define ADDR_BIT2_PIN       8
#define ADDR_BIT3_PIN       9   // MSB

// ROMからのデータ出力用のピン
#define ROM_BIT0_PIN       10   // LSB
#define ROM_BIT1_PIN       11
#define ROM_BIT2_PIN       12
#define ROM_BIT3_PIN       13
#define ROM_BIT4_PIN       A1   // デジタル出力として使用
#define ROM_BIT5_PIN       A2
#define ROM_BIT6_PIN       A3
#define ROM_BIT7_PIN       A4   // MSB

#define ROM_SIZE           16
#define RESET_DELAY_MS     500
#include <limits.h>
#define INITIAL_LAST_CLOCK_TIME INT_MIN

// コピペ用命令一覧
// MOV A, Im
//   B00110000,
// MOV B, im
//   B01110000,
// MOV A, B
//   B00010000,
// MOV B, A
//   B01000000,
// ADD A, Im
//   B00000000,
// ADD B, Im
//   B01010000,
// IN A
//   B00100000,
// IN B
//   B01100000,
// OUT Im
//   B10110000,
// OUT B
//   B10010000,
// JMP Im
//   B11110000,
// JNC Im
//   B11100000,

// ROMデータ（命令テスト: 転送）
// unsigned char rom[] = {
//   // Aを111に
//   B00110111,

//   // 結果をBレジスタに移す
//   B01000000,

//   // Bレジスタの値を出力する
//   B10010000,

//   // レジスタと出力ポートを0に
//   B00110000,
//   B01110000,
//   B10110000,

//   // 0行目へジャンプ
//   B11110000,

//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
// };

// ROMデータ（命令テスト: 加算）
// unsigned char rom[] = {
//   // Aレジスタで0101+0010
//   B00110101,
//   B00000010,

//   // 結果をBレジスタに移す
//   B01000000,

//   // Bレジスタの値を出力する
//   B10010000,

//   // レジスタと出力ポートを0に
//   B00110000,
//   B01110000,
//   B10110000,

//   // 0行目へジャンプ
//   B11110000,

//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
// };

// ROMデータ（命令テスト: In A）
// unsigned char rom[] = {
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
//   B00100000,
// };

// ROMデータ（Lチカ）
// unsigned char rom[] = {
//   B10110011,
//   B10110110,
//   B10111100,
//   B10111000,
//   B10111000,
//   B10111100,
//   B10110110,
//   B10110011,
//   B10110001,
//   B11110000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
//   B00000000,
// };

// ROMデータ（ラーメンタイマー）
unsigned char rom[] = {
  B10110111,
  B00000001,
  B11100001,
  B00000001,
  B11100011,
  B10110110,
  B00000001,
  B11100110,
  B00000001,
  B11101000,
  B10110000,
  B10110100,
  B00000001,
  B11101010,
  B10111000,
  B11111111,
};

// クロック制御
unsigned long lastClockTime = INITIAL_LAST_CLOCK_TIME;
unsigned long clockInterval = 100;
bool clockState = LOW;

// システム状態
unsigned char currentAddress = 0;
unsigned char currentInstruction = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(CLOCK_OUTPUT_PIN, OUTPUT);
  pinMode(RESET_OUTPUT_PIN, OUTPUT);
  
  pinMode(ADDR_BIT0_PIN, INPUT);
  pinMode(ADDR_BIT1_PIN, INPUT);
  pinMode(ADDR_BIT2_PIN, INPUT);
  pinMode(ADDR_BIT3_PIN, INPUT);
  
  pinMode(ROM_BIT0_PIN, OUTPUT);
  pinMode(ROM_BIT1_PIN, OUTPUT);
  pinMode(ROM_BIT2_PIN, OUTPUT);
  pinMode(ROM_BIT3_PIN, OUTPUT);
  pinMode(ROM_BIT4_PIN, OUTPUT);
  pinMode(ROM_BIT5_PIN, OUTPUT);
  pinMode(ROM_BIT6_PIN, OUTPUT);
  pinMode(ROM_BIT7_PIN, OUTPUT);
  
  digitalWrite(CLOCK_OUTPUT_PIN, LOW);
  digitalWrite(RESET_OUTPUT_PIN, HIGH);
  
  reset();
}

void loop() { 
  generateClock();
  delay(1);
}

void updateROMOutput() {
  // プログラムカウンタ読み取り
  int addr_bit0 = digitalRead(ADDR_BIT0_PIN);
  int addr_bit1 = digitalRead(ADDR_BIT1_PIN);
  int addr_bit2 = digitalRead(ADDR_BIT2_PIN);
  int addr_bit3 = digitalRead(ADDR_BIT3_PIN);
  
  currentAddress = (addr_bit3 << 3) | (addr_bit2 << 2) | (addr_bit1 << 1) | addr_bit0;
  if (currentAddress >= ROM_SIZE) {
    Serial.println("エラー: 無効なアドレス");
    return;
  }
  
  currentInstruction = rom[currentAddress];

  // ROMからプログラムカウンタが指す行のプログラムを出力する
  digitalWrite(ROM_BIT0_PIN, (currentInstruction >> 0) & 1);
  digitalWrite(ROM_BIT1_PIN, (currentInstruction >> 1) & 1);
  digitalWrite(ROM_BIT2_PIN, (currentInstruction >> 2) & 1);
  digitalWrite(ROM_BIT3_PIN, (currentInstruction >> 3) & 1);
  digitalWrite(ROM_BIT4_PIN, (currentInstruction >> 4) & 1);
  digitalWrite(ROM_BIT5_PIN, (currentInstruction >> 5) & 1);
  digitalWrite(ROM_BIT6_PIN, (currentInstruction >> 6) & 1);
  digitalWrite(ROM_BIT7_PIN, (currentInstruction >> 7) & 1);
}

bool shouldUpdateClock() {
  return (millis() - lastClockTime >= clockInterval);
}

void generateClock() {
  unsigned long currentTime = millis();

  if (!shouldUpdateClock()) {
    return;
  }

  clockState = !clockState;
  digitalWrite(CLOCK_OUTPUT_PIN, clockState);
  lastClockTime = currentTime;
  
  // クロックがLからHに変わった場合（クロックが立ち上がった場合）の処理
  if (clockState == HIGH) {
    updateROMOutput();
    printSystemStatus();
  }
}

void reset() {
  Serial.println("システムリセット実行");
  // レジスタには、書籍と異なり74HC163を使っている
  // 74HC163をリセットするには、CLRがLの状態でクロックを立ち上げる必要がある
  digitalWrite(CLOCK_OUTPUT_PIN, LOW);
  digitalWrite(RESET_OUTPUT_PIN, LOW);
  delay(RESET_DELAY_MS);

  // 初回のクロック立ち上げ
  generateClock();
  digitalWrite(RESET_OUTPUT_PIN, HIGH);
}

void printSystemStatus() {
  Serial.print("アドレス: ");
  printBinary(currentAddress, 4);
  Serial.print(" (");
  Serial.print(currentAddress);
  Serial.print(") | 命令: ");
  printBinary(currentInstruction, 8);
  Serial.print(") | 周期: ");
  Serial.print(clockInterval * 2);
  Serial.print("ms");
  Serial.println();
}

void printBinary(unsigned char value, int bits) {
  for (int i = bits - 1; i >= 0; i--) {
    Serial.print((value >> i) & 1);
  }
}