#include"HexFunc.h"


string byteToHex(BYTE& b){
	return HEX_DIGITS[b / 16] + HEX_DIGITS[b % 16];
}

BYTE hexCharToByte(char& hex){

	if(hex >= '0' && hex <= '9')
		return hex - '0';

	if(hex >= 'a' && hex <= 'f')
		return 10 + (hex - 'a');

	return -1;
}

BYTE hexStringToByte(string& hex){
	return hexCharToByte(hex[0]) * 16 + hexCharToByte(hex[1]);
}
