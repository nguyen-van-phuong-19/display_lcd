#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

LiquidCrystal_I2C I2C_LCD1(0x27, 20, 4);

void clear_screen();
void display(String text, int row, int column);
void screen_0();
void screen_1();
void screen_2();
void screen_3();
int check_the_btn(int *state, int *last_state, int pin);
void set_start_time();
void update_time();
void update_egg_incubation_time();
void set_option_egg_incubation();



const int button_down_pin = 4;
const int button_ok_pin = 16;
const int button_up_pin = 17;

int button_down_state = 1;
int button_ok_state = 1;
int button_up_state = 1;

int last_button_down_state = 1;
int last_button_ok_state = 1;
int last_button_up_state = 1;

int screen = 0;
int a = 0;
int hours;
int start_time = 0;
int period = 8;
int last_hours = 0;
int day_1 = start_time;
int egg_incubation_time = 21 * 24;
int day_2 = egg_incubation_time / 24;
int hours_2 = egg_incubation_time % 24;
int temperature = 37;
bool egg_incubation = false;
bool pr_egg_incubation = egg_incubation;
int current_position = 0;
int select_option = 0;
int select_option_1 = 0;
String option_screen_1[4] = {"CHON LOAI TRUNG", "TUY CHINH", "DAT TG BAT DAU AP", "QUAY LAI"};
String option_screen_2_1[5] = {"TRUNG 1", "TRUNG 2", "TRUNG 3", "TRUNG 4", "QUAY LAI"};
String option_screen_2_2[3] = {"NHIET DO", "THOI GIAN AP", "QUAY LAI"};
int egg_index_list[4][2] = {
  {504, 37},
  {505, 39},
  {506, 38},
  {506, 36}
};

void setup()
{

  Serial.begin(115200);

  pinMode(button_down_pin, INPUT);
  pinMode(button_ok_pin, INPUT);
  pinMode(button_up_pin, INPUT);

  I2C_LCD1.init();
  I2C_LCD1.backlight();
  display("Hello World!", 0, 0);
  display("display done!", 1, 0);

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }

  timeClient.begin();
  clear_screen();
  set_start_time();
}


void loop()
{
  switch (screen) {
    case 0:
      screen_0();
      break;
    case 1:
      screen_1();
      break;
    case 2:
      screen_2();
      break;
    case 3:
      screen_3();
      break;
    default:
      break;
  }
  // timeClient.update();

  // Serial.println(timeClient.getFormattedTime());
  delay(50);
}

void clear_screen() {
  I2C_LCD1.clear();
}

void display(String text, int row, int column) {
  // I2C_LCD1.clear();
  I2C_LCD1.setCursor(column, row);
  I2C_LCD1.print(text);
}



void screen_0() {
  // Serial.println(egg_incubation);
  // clear_screen();
  hours = timeClient.getHours();
  if (egg_incubation == true) {
    display("THOI GIAN AP CON:", 0, 0);
    display(String(day_2), 1, 0);
    display("NGAY", 1, 3);
    display(String(hours_2), 1, 8);
    display("GIO", 1, 11);
  } else {
    display("BAT DAU AP SAU:", 0, 0);
    display(String(day_1), 1, 0);
    display("NGAY", 1, 3);
    display(String(start_time), 1, 8);
    display("GIO", 1, 11);
  }
  display(timeClient.getFormattedTime(), 2, 0);
  display("NHIET DO:", 3, 0);
  display(String(temperature), 3, 10);
  update_time();
  if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
    clear_screen();
    screen = 1;
    current_position = 0;
  }
}

void screen_1() {

  for (int i = 0; i < 4; i++) {
    if (i == current_position) {
      display("+ ", i, 0);
      display(option_screen_1[i], i, 2);
    } else {
      display(option_screen_1[i] + "  ", i, 0);
    }
  }

  if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {
    if (current_position != 3) {
      current_position++;
    }
  }
  if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
    if (current_position != 0) {
      current_position--;
    }
  }
  if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
    if (current_position == 3) {
      clear_screen();
      select_option = current_position;
      screen = 0;
    } else {
      clear_screen();
      select_option = current_position;
      screen = 2;
    }
    current_position = 0;
  }
}

void screen_2() {
  switch (select_option) {
    case 0:
      for (int i = 0; i < 5; i++) {
        if (i == current_position) {
          if (current_position == 0 || current_position == 1 || current_position == 2) {
            display("+ ", i, 0);
            display(option_screen_2_1[i], i, 2);
          } else {
            display("+ ", i - 1, 0);
            display(option_screen_2_1[i], i - 1, 2);
          }
        } else {
          if (current_position == 0 || current_position == 1 || current_position == 2) {
            if (i < 4) {
              display(option_screen_2_1[i] + "  ", i, 0);
            }
          } else {
            if (i - 1 >= 0) {
              display(option_screen_2_1[i] + "  ", i - 1, 0);
            }
          }
        }
      }
      if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {
        if (current_position != 4) {
          current_position++;
        }
      }
      if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
        if (current_position != 0) {
          current_position--;
        }
      }
      if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
        if (current_position == 4) {
          clear_screen();
          select_option_1 = current_position;
          screen = 1;
        } else {
          clear_screen();
          select_option_1 = current_position;
          set_option_egg_incubation();
          screen = 0;
        }
        current_position = 0;
      }
      break;
    case 1:
      for (int i = 0; i < 3; i++) {
        if (i == current_position) {
          display("+ ", i, 0);
          display(option_screen_2_2[i], i, 2);
        } else {
          display(option_screen_2_2[i] + "  ", i, 0);
        }
      }

      if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {
        if (current_position != 2) {
          current_position++;
        }
      }
      if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
        if (current_position != 0) {
          current_position--;
        }
      }
      if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
        if (current_position == 2) {
          clear_screen();
          select_option_1 = current_position;
          screen = 1;
        } else {
          clear_screen();
          select_option_1 = current_position;
          screen = 3;
        }
        current_position = 0;
      }
      break;
    case 2:
      display(String(period), 3, 15);

      if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {

        if (period > 0) {
          period--;
        }


      }
      if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
        period++;
      }
      if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
        clear_screen();
        set_start_time();
        screen = 0;
      }
      break;
    case 3:
      break;
    default:
      break;
  }

}

void screen_3() {
  switch (select_option_1) {
    case 0:
      display(String(temperature), 3, 15);

      if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {


        if (temperature > 0) {
          temperature--;
        }
      }
      if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
        temperature++;
      }
      if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
        clear_screen();
        screen = 0;
      }
      break;
    case 1:
      display(String(egg_incubation_time), 3, 15);

      if (check_the_btn(&button_down_state, &last_button_down_state, button_down_pin) == 1) {

        if (egg_incubation_time > 0) {
          egg_incubation_time--;
        }
      }
      if (check_the_btn(&button_up_state, &last_button_up_state, button_up_pin) == 1) {
        egg_incubation_time++;
      }
      if (check_the_btn(&button_ok_state, &last_button_ok_state, button_ok_pin) == 1) {
        update_egg_incubation_time();
        clear_screen();
        screen = 0;
      }
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

int check_the_btn(int *state, int *last_state, int pin) {
  *state = digitalRead(pin);
  if (*state != *last_state) {
    *last_state = *state;
    if (*state == HIGH) {
      return 1;
    }
  }
  return 0;
}

void set_start_time() {
  day_1 = period / 24;
  start_time = period % 24;
}

void update_time() {
  hours = timeClient.getHours();
  if (egg_incubation == false) {
    if (start_time == 0 && day_1 == 0) {
      egg_incubation = true;
      clear_screen();
    }
  }
  if (hours != last_hours) {
    if (hours_2 == 0) {
      hours_2 = 24;
      day_2--;
    }
    egg_incubation_time--;
    hours_2--;
    if (start_time == 0 && day_1 > 0) {
      start_time = 24;
      day_1--;
      start_time--;
    } else if (start_time > 0 && day_1 >= 0) {
      start_time--;
    }
    if (hours == 0 && last_hours == 23) {
      if (day_2 > 0) {
        day_2--;
      }
      if (day_1 > 0) {
        day_1--;
      }
    }
  }
  pr_egg_incubation = egg_incubation;
  last_hours = hours;
}

void update_egg_incubation_time() {
  day_2 = egg_incubation_time / 24;
  hours_2 = egg_incubation_time % 24;
}

void set_option_egg_incubation() {
  for (int i = 0; i < 4; i++) {
    if (i == select_option_1) {
      egg_incubation_time = egg_index_list[i][0];
      temperature = egg_index_list[i][1];
      update_egg_incubation_time();
    }
  }
}

