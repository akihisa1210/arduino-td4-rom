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

// ROMデータ（8ビット×16行）
unsigned char rom[] = {
  B00000000,  // アドレス 0
  B00000001,  // アドレス 1
  B00000011,  // アドレス 2
  B00000111,  // アドレス 3
  B00001111,  // アドレス 4
  B00011111,  // アドレス 5
  B00111111,  // アドレス 6
  B01111111,  // アドレス 7
  B11111111,  // アドレス 8
  B11111110,  // アドレス 9
  B11111100,  // アドレス 10
  B11111000,  // アドレス 11
  B11110000,  // アドレス 12
  B11100000,  // アドレス 13
  B11000000,  // アドレス 14
  B10000000,  // アドレス 15
};

// クロック制御変数
unsigned long lastClockTime = 0;
unsigned long clockInterval = 500;
bool clockState = LOW;
bool systemReset = false;

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
  readROM();
  
  generateClock();
  
  delay(1); // 短いディレイで安定性向上
}

void readROM() {
  int addr_bit0 = digitalRead(ADDR_BIT0_PIN);
  int addr_bit1 = digitalRead(ADDR_BIT1_PIN);
  int addr_bit2 = digitalRead(ADDR_BIT2_PIN);
  int addr_bit3 = digitalRead(ADDR_BIT3_PIN);
  
  currentAddress = (addr_bit3 << 3) | (addr_bit2 << 2) | (addr_bit1 << 1) | addr_bit0;
  
  currentInstruction = rom[currentAddress];
  
  digitalWrite(ROM_BIT0_PIN, (currentInstruction >> 0) & 1);
  digitalWrite(ROM_BIT1_PIN, (currentInstruction >> 1) & 1);
  digitalWrite(ROM_BIT2_PIN, (currentInstruction >> 2) & 1);
  digitalWrite(ROM_BIT3_PIN, (currentInstruction >> 3) & 1);
  digitalWrite(ROM_BIT4_PIN, (currentInstruction >> 4) & 1);
  digitalWrite(ROM_BIT5_PIN, (currentInstruction >> 5) & 1);
  digitalWrite(ROM_BIT6_PIN, (currentInstruction >> 6) & 1);
  digitalWrite(ROM_BIT7_PIN, (currentInstruction >> 7) & 1);
}

void generateClock() {
  unsigned long currentTime = millis();
  if (currentTime - lastClockTime >= clockInterval) {
    clockState = !clockState;
    digitalWrite(CLOCK_OUTPUT_PIN, clockState);
    lastClockTime = currentTime;
    
    // 立ち上がりエッジでデバッグ出力
    if (clockState == HIGH) {
      printSystemStatus();
    }
  }
}

void reset() {
  // プログラムカウンタをリセット
  digitalWrite(RESET_OUTPUT_PIN, LOW);  // リセット実行
  delay(10);
  digitalWrite(RESET_OUTPUT_PIN, HIGH); // リセット解除
  delay(10);
  
  // クロック状態リセット
  clockState = LOW;
  digitalWrite(CLOCK_OUTPUT_PIN, LOW);
  
  Serial.println("システムリセット実行");
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