

int isDigit(unsigned char ch){
    if(ch>= '0' && ch <= '9'){
        return 1;
    }else{
        return 0;
    }
}


unsigned char character_to_hex(unsigned char highByte, unsigned char lowByte){
    highByte = isDigit(highByte)? (highByte - '0'):(highByte - 'A' + 10);
    lowByte = isDigit(lowByte)? (lowByte - '0'):(lowByte - 'A' + 10);
    return ((highByte<<4) + lowByte);
}


char *urldecode(char *str){
    int length = strlen(str);
    if(length == 0) return "";
    
    char *result = (char *)malloc(length);
    int i=0, j=0;
    for(i=0;i<length;i++){
        if(str[i] == '%'){
            result[j] = character_to_hex(str[i+1], str[i+2]);
            i+=2;
        }else if(str[i] == '+'){
            result[j] = ' ';
        }else{
            result[j] = str[i];
        }
        j++;
    }
    
    return result;
}


