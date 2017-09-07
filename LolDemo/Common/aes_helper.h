#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <openssl/aes.h>
#include <windows.h>

std::string Encoder(std::string srctext, std::string key);
std::string Decoder(std::string strctext, std::string key);
void ControlEnLength(std::string &strtext);
void ControlDeLength(std::string &strtext);
int aes_decrypt(char* in, unsigned int size, char* key, char* out);//º”√‹
int aes_encrypt(char* in, unsigned int size, char* key, char* out);//Ω‚√‹
