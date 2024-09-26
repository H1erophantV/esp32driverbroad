#include "Wire.h"
#define MAX_SIZE 20
char receive_buff[MAX_SIZE];
#define I2C_DEV_ADDR 0x08
char device_type;
int device_index,pwm;

int strlen_custom(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Function to copy a substring from a string
void strncpy_custom(char *dest, const char *src, int length) {
    for (int i = 0; i < length; i++) {
        dest[i] = src[i];
    }
    dest[length] = '\0';
}

// Count the number of occurrences of a delimiter in the string
int count_delimiters(const char *str, char delimiter) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == delimiter) {
            count++;
        }
    }
    return count;
}

// Split the string based on the delimiter
char** split(const char* str, char delimiter) {
  int num_tokens;
    if (str == NULL) {
        num_tokens = 0;
        return NULL;
    }

    int str_len = strlen_custom(str);
    int delimiters_count = count_delimiters(str, delimiter);

    // The number of resulting tokens is delimiters_count + 1
    num_tokens = delimiters_count + 1;

    // Allocate memory for the array of strings
    char** result = (char**)malloc((num_tokens) * sizeof(char*));

    int start = 0; // Start index of a token
    int token_index = 0;

    // Iterate over the string to split it
    for (int i = 0; i <= str_len; i++) {
        if (str[i] == delimiter || str[i] == '\0') {
            int token_length = i - start;
            result[token_index] = (char*)malloc((token_length + 1) * sizeof(char));
            strncpy_custom(result[token_index], str + start, token_length);
            token_index++;
            start = i + 1; // Move start to the next character after the delimiter
        }
    }

    return result;
}

int string2num(const char *str) {
    // Initialize variables
    int result = 0;
    int sign = 1;  // Default is positive

    // Check if the string is NULL or empty
    if (str == NULL || *str == '\0') {
        return 0;  // Return 0 for invalid input
    }

    // Handle optional leading whitespace
    while (*str == ' ') {
        str++;
    }

    // Check for sign indicator
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Process each digit
    while (*str != '\0') {
        // Check if the character is a valid digit
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        } else {
            // If a non-numeric character is found, stop processing
            break;
        }
        str++;
    }

    // Apply the sign
    result *= sign;

    return result;
}



void IIC_Start(){//default use 21 as SDA and 22 as SCL default addr 0x08
  Wire.begin();
}

void IIC_listen(){
  Wire.beginTransmission(I2C_DEV_ADDR);

  uint8_t error = Wire.endTransmission(true);
  Serial.printf("endTransmission: %u\n", error);//Transmission confirm,print error to serial when error
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, MAX_SIZE);//return received length
  if ((bool)bytesReceived) {
       Wire.readBytes(receive_buff, bytesReceived);//receive iic bytes
       Serial.println(receive_buff);
  }
}
void memory_erase(){
  for(int i = 0 ;i< MAX_SIZE;i++){
    receive_buff[i] = 0;
  }

}


//message type (char)s/m(device type) (int)index(device index) (int)pwm
//eg.message from host:"s 1 1500",set servo1 output 1500
//the board has 4 motor channels and 8 servo channels
void message_decode(const char * message){
  int num_tokens;
  char ** control_message;
  control_message = split(message , ' ');
  device_type = control_message[0][0];
  device_index = string2num(control_message[1]);
  pwm = string2num(control_message[2]);

}

void setup() {
  // put your setup code here, to run once:
  IIC_Start();
  Serial.begin(115200);//Serial1 print IIC status,default baud 115200

}

void loop() {

  IIC_listen();

}
