// D5（MSB）、D4、D3、D2（LSB）: プログラムカウンタからのアドレス入力
// D13（MSB）、D12、D11、D10、D9、D8、D7、D6（LSB）: ROMからの命令出力
// D0、D1: デバッグ用のシリアル通信のため開けておく

unsigned char rom[] = {
  B00000000,
  B00000001,
  B00000010,
  B00000011,
  B00000100,
  B00000101,
  B00000110,
  B00000111,
  B00001000,
  B00001001,
  B00001010,
  B00001011,
  B00001100,
  B00001101,
  B00001110,
  B00001111,
};

void setup() {
  // シリアル通信開始（デバッグ用）
  Serial.begin(9600);

  // アドレス入力ピンを入力用に設定
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);

  // 命令出力ピンを出力用に設定
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.println("TD4 ROM エミュレーター開始");
  Serial.println("");
}

void loop() {
  // プログラムカウンタからのアドレス入力を読み取る
  int addr_bit0 = digitalRead(2); // LSB
  int addr_bit1 = digitalRead(3);
  int addr_bit2 = digitalRead(4);
  int addr_bit3 = digitalRead(5); // MSB
  int address = (addr_bit3 << 3) | (addr_bit2 << 2) | (addr_bit1 << 1) | addr_bit0;

  Serial.print("PC=");
  Serial.print(address, BIN);
  Serial.println("");

  // ROMからデータを読み取る
  unsigned char instruction = rom[address];
  int inst_bit0 = (instruction >> 0) & 1;
  int inst_bit1 = (instruction >> 1) & 1;
  int inst_bit2 = (instruction >> 2) & 1;
  int inst_bit3 = (instruction >> 3) & 1;
  int inst_bit4 = (instruction >> 4) & 1;
  int inst_bit5 = (instruction >> 5) & 1;
  int inst_bit6 = (instruction >> 6) & 1;
  int inst_bit7 = (instruction >> 7) & 1;

  Serial.print("命令=");
  Serial.print(instruction, BIN);
  Serial.println("");

  // 命令を出力
  digitalWrite(6, inst_bit0); // LSB
  digitalWrite(7, inst_bit0);
  digitalWrite(8, inst_bit0);
  digitalWrite(9, inst_bit0);
  digitalWrite(10, inst_bit0);
  digitalWrite(11, inst_bit0);
  digitalWrite(12, inst_bit0);
  digitalWrite(13, inst_bit0); // MSB
}